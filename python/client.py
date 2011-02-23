import wx
import socketlib
import socket
import asyncore

APP_ABOUT = """Robot Client Beta
Dan Quinlivan

Begin by connecting to the robot with File -> connect

w - accelerate
s - decelerate
a - turn left
d - turn right

"""

robotserver = {'host': 'localhost', 'port': 8080}

class RobotClient(wx.Panel):
    def __init__(self, parent, id):
        
        wx.Panel.__init__(self, parent, id, wx.DefaultPosition,wx.DefaultSize, wx.WANTS_CHARS)
        self.proxy_connection = False
        self.keypress = {}
        self.throttleControl = ThrottleControl()
        
        # window sizers
        mainSizer = wx.BoxSizer(wx.VERTICAL)
        grid = wx.GridBagSizer(hgap=5, vgap=5)
        hSizer = wx.BoxSizer(wx.HORIZONTAL)
                
        # create motor fields
        motor_fields = ['V-target', 'V-actual', 'POWER', 'RPM', 'PWM', 'FAULT']
        self.fields = {}
        row = 0
        txtfieldsize = 75
        for field in motor_fields:
            self.fields[field] = {}
            self.fields[field]['name'] = wx.StaticText(self, label=field)
            self.fields[field]['left'] = wx.TextCtrl(self, size=(txtfieldsize,-1), style=wx.TE_READONLY)
            self.fields[field]['right'] = wx.TextCtrl(self, size=(txtfieldsize,-1), style=wx.TE_READONLY)
            grid.Add(self.fields[field]['name'], pos=(row,1), flag=wx.ALIGN_CENTER )
            grid.Add(self.fields[field]['left'], pos=(row,0))
            grid.Add(self.fields[field]['right'], pos=(row,2))
            wx.EVT_SET_FOCUS(self.fields[field]['left'], self.keepKeyboardInputFocus)
            wx.EVT_SET_FOCUS(self.fields[field]['right'], self.keepKeyboardInputFocus)
            row += 1

        
        # Throttle Sliders
        self.leftThrottle = wx.Slider(self, 100, 0, -100, 100, (0,0), (70, 200), wx.SL_VERTICAL | wx.SL_AUTOTICKS | wx.SL_LABELS | wx.SL_INVERSE | wx.SL_LEFT)
        self.rightThrottle = wx.Slider(self, 200, 0, -100, 100, (0,0), (70, 200), wx.SL_VERTICAL | wx.SL_AUTOTICKS | wx.SL_LABELS | wx.SL_INVERSE | wx.SL_RIGHT)
        self.leftThrottle.SetTickFreq(10)
        self.rightThrottle.SetTickFreq(10)
        wx.EVT_SET_FOCUS(self.leftThrottle, self.keepKeyboardInputFocus)
        wx.EVT_SET_FOCUS(self.rightThrottle, self.keepKeyboardInputFocus)
        
        
        # Keyboard Input
        self.keyboardinputbutton = wx.Button(self, label="Keyboard Input")        
        wx.EVT_KEY_DOWN(self.keyboardinputbutton, self.onKeyDown)    
        wx.EVT_KEY_UP(self.keyboardinputbutton, self.onKeyUp)    
        self.keyboardinputbutton.SetFocus();
        
        # Logging panel
        self.logger = wx.TextCtrl(self, size=(300,300), style=wx.TE_MULTILINE | wx.TE_READONLY)
        wx.EVT_SET_FOCUS(self.logger, self.keepKeyboardInputFocus)

        # wxTimer events
        self.poller = wx.Timer(self, wx.NewId())
        self.Bind(wx.EVT_TIMER, self.OnPoll)
        self.poller.Start(20, wx.TIMER_CONTINUOUS)
    
        
        # arrange widgets
        hSizer.Add(self.leftThrottle)
        hSizer.Add(grid, 0, wx.ALL, 5)
        hSizer.Add(self.rightThrottle)
        hSizer.Add(self.keyboardinputbutton)
        hSizer.AddSpacer(10)
        hSizer.Add(self.logger)
        mainSizer.Add(hSizer, 0, wx.ALL, 5)
        
        self.SetSizerAndFit(mainSizer)
        
    def keepKeyboardInputFocus(self, event):
        self.keyboardinputbutton.SetFocus();
    
    def onKeyDown(self, event):
        key = event.GetKeyCode()
        self.keypress[key] = 1
    
    def onKeyUp(self, event):
        key = event.GetKeyCode()
        self.keypress[key] = 0
    
    def OnPoll(self, event):        

        # handle keyboard input
        keys = self.throttleControl.key_mapping.keys()
        for key in keys:
            key_ord = ord(key)

            if self.keypress.get(key_ord, None) == 1:
                self.throttleControl.key_mapping[key]()
                self.leftThrottle.SetValue(self.throttleControl.throttle['left'])
                self.rightThrottle.SetValue(self.throttleControl.throttle['right'])
        
        # socket work
        asyncore.poll(timeout=0)
        if self.proxy_connection != False:
            if self.proxy_connection.is_open() == False:
                self.proxy_connection = ProxyConnection(robotserver['host'], int(robotserver['port']))
            if self.proxy_connection.is_open() == True:
                pass
                #self.proxy_connection.write("echo,1,2")
                
        
class ThrottleControl:
    
    def __init__(self):
        self.key_mapping = {'W': self.accelerate, 'S': self.decelerate, 'A': self.turnleft, 'D': self.turnright}
        self.throttle = {'left': 0.0, 'right': 0.0}
        
    def accelerate(self):
        self.throttle['left'] += 1
        self.throttle['right'] += 1
        self.validate()
    
    def decelerate(self):
        self.throttle['left'] -= 1
        self.throttle['right'] -= 1
        self.validate()
    
    def turnleft(self):
        self.throttle['left'] -= 1
        self.throttle['right'] += 1
        self.validate()
        
    def turnright(self):
        self.throttle['left'] += 1
        self.throttle['right'] -= 1
        self.validate()
        
    def validate(self):
        if self.throttle['left'] > 100: self.throttle['left'] = 100
        if self.throttle['left'] < -100: self.throttle['left'] = -100
        if self.throttle['right'] > 100: self.throttle['right'] = 100
        if self.throttle['right'] < -100: self.throttle['right'] = -100
        
        
class MainWindow(wx.Frame):
    def __init__(self, parent, title):
        
        self.ID_CONNECT = 5000
        self.ID_DISCONNECT = 5001
        
        wx.Frame.__init__(self, parent, title=title, size=(820,430))
        self.statusbar = self.CreateStatusBar() # A StatusBar in the bottom of the window
        self.statusbar.PushStatusText("Disconnected... use File -> Connect to connect to the robot")

        # Create menus
        filemenu= wx.Menu()
        helpmenu= wx.Menu()

        # Build menu items
        menuAbout = helpmenu.Append(wx.ID_ABOUT, "&About"," Information about this program")
        menuConnect = filemenu.Append(self.ID_CONNECT, "&Connect..."," Open connection to robot")
        menuDisconnect = filemenu.Append(self.ID_DISCONNECT, "&Disconnect"," Disconnect from the robot")
        menuExit = filemenu.Append(wx.ID_EXIT,"&Exit"," Terminate the program")     

        # Attach menus to menu bar
        menuBar = wx.MenuBar()
        menuBar.Append(filemenu,"&File") # Adding the "filemenu" to the MenuBar
        menuBar.Append(helpmenu,"&Help") # Adding the "filemenu" to the MenuBar
        self.SetMenuBar(menuBar)  # Adding the MenuBar to the Frame content.

        # Bind Menu Events
        self.Bind(wx.EVT_MENU, self.OnAbout, menuAbout)
        self.Bind(wx.EVT_MENU, self.OnExit, menuExit)
        self.Bind(wx.EVT_MENU, self.OnConnect, menuConnect)
        self.Bind(wx.EVT_MENU, self.OnDisconnect, menuDisconnect)

        self.Show(True)

    def OnAbout(self,e):
        # show about dialog
        dlg = wx.MessageDialog( self, APP_ABOUT, "About Robot Client", wx.OK)
        dlg.ShowModal() 
        dlg.Destroy()

    def OnExit(self,e):
        self.Close(True)
        
    def OnConnect(self,e):
        # connect to robot dialog box
        self.OnDisconnect(self)
        dlg = wx.TextEntryDialog(
                self, 'Enter <server>:<port> to connect',
                'Open connection to robot', str(robotserver['host']) + ':' + str(robotserver['port']))

        if dlg.ShowModal() == wx.ID_OK:
            result = dlg.GetValue()
            (robotserver['host'],robotserver['port']) = result.split(':')
            robot_panel.logger.AppendText("Connecting to " + robotserver['host'] + ":" + robotserver['port'] + "...\n")
            proxy_connection = ProxyConnection(robotserver['host'], int(robotserver['port']))
            robot_panel.proxy_connection = proxy_connection           

        dlg.Destroy()
    
    def OnDisconnect(self, e):
        # disconnect from robot menu option
        if robot_panel.proxy_connection != False:
            robot_panel.logger.AppendText("Terminating connection...\n")
            robot_panel.proxy_connection.close()
            robot_panel.proxy_connection = False

# socket connection to robot proxy
class ProxyConnection(socketlib.Socket):
    def __init__(self, host, port):
        socketlib.Socket.__init__(self)
        self.create_socket(socket.AF_INET, socket.SOCK_STREAM)
        self.connect( (host, port) )
        self.set_terminator("\r\n")
        self.open = True
        
    def write(self, data):
        socketlib.Socket.write(self, data)        
                
    def handle_request(self, req):
        robot_panel.logger.AppendText(req + "\n")
        
    def handle_close(self):
        robot_panel.logger.AppendText("Connection Timeout\n")
        socketlib.Socket.handle_close(self)
        
    def handle_connect(self):
        robot_panel.logger.AppendText("Connection established!\n")
            
app = wx.App(False)
frame = MainWindow(None, "6WD Robot Client")
robot_panel = RobotClient(frame, 1)
frame.Show()
app.MainLoop()        