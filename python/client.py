import wx
import socketlib

OUTPUTFIELD_SIZE = 75

class RobotClient(wx.Panel):
    def __init__(self, parent, id):
        
        wx.Panel.__init__(self, parent, id, wx.DefaultPosition,wx.DefaultSize, wx.WANTS_CHARS)
        
        self.keypress = {}
        
        self.throttleControl = ThrottleControl()
        
        # create some sizers
        mainSizer = wx.BoxSizer(wx.VERTICAL)
        grid = wx.GridBagSizer(hgap=5, vgap=5)
        hSizer = wx.BoxSizer(wx.HORIZONTAL)
                
        motor_fields = ['V-target', 'V-actual', 'POWER', 'RPM', 'PWM', 'FAULT']
        
        self.fields = {}
        row = 0
        for field in motor_fields:
            self.fields[field] = {}
            self.fields[field]['name'] = wx.StaticText(self, label=field)
            self.fields[field]['left'] = wx.TextCtrl(self, size=(OUTPUTFIELD_SIZE,-1), style=wx.TE_READONLY)
            self.fields[field]['right'] = wx.TextCtrl(self, size=(OUTPUTFIELD_SIZE,-1), style=wx.TE_READONLY)
            grid.Add(self.fields[field]['name'], pos=(row,1), flag=wx.ALIGN_CENTER )
            grid.Add(self.fields[field]['left'], pos=(row,0))
            grid.Add(self.fields[field]['right'], pos=(row,2))
            wx.EVT_SET_FOCUS(self.fields[field]['left'], self.keepKeyboardInputFocus)
            wx.EVT_SET_FOCUS(self.fields[field]['right'], self.keepKeyboardInputFocus)
            row += 1

        
        # Throttle Sliders
        self.leftThrottle = wx.Slider(
            self, 100, 0, -100, 100, (0,0), (70, 200), 
            wx.SL_VERTICAL | wx.SL_AUTOTICKS | wx.SL_LABELS | wx.SL_INVERSE | wx.SL_LEFT
            )
            
        self.rightThrottle = wx.Slider(
            self, 200, 0, -100, 100, (0,0), (70, 200), 
            wx.SL_VERTICAL | wx.SL_AUTOTICKS | wx.SL_LABELS | wx.SL_INVERSE | wx.SL_RIGHT
            )            

        self.leftThrottle.SetTickFreq(10)
        self.rightThrottle.SetTickFreq(10)
        wx.EVT_SET_FOCUS(self.leftThrottle, self.keepKeyboardInputFocus)
        wx.EVT_SET_FOCUS(self.rightThrottle, self.keepKeyboardInputFocus)
        
        
        # Keyboard Input
        self.keyboardinputbutton = wx.Button(self, label="Keyboard Input")        
        wx.EVT_KEY_DOWN(self.keyboardinputbutton, self.onKeyDown)    
        wx.EVT_KEY_UP(self.keyboardinputbutton, self.onKeyUp)    
        
        
        
        
        # A multiline TextCtrl - This is here to show how the events work in this program, don't pay too much attention to it
        self.logger = wx.TextCtrl(self, size=(200,300), style=wx.TE_MULTILINE | wx.TE_READONLY)
        wx.EVT_SET_FOCUS(self.logger, self.keepKeyboardInputFocus)

        # A button
        self.button =wx.Button(self, label="Save")
        self.Bind(wx.EVT_BUTTON, self.OnClick,self.button)
        

        # wxTimer events
        self.poller = wx.Timer(self, wx.NewId())
        self.Bind(wx.EVT_TIMER, self.OnPoll)
        #poll 50 times/second, should be enough for low-bandwidth apps
        self.poller.Start(20, wx.TIMER_CONTINUOUS)
    
        
        hSizer.Add(self.leftThrottle)
        hSizer.Add(grid, 0, wx.ALL, 5)
        hSizer.Add(self.rightThrottle)
        hSizer.Add(self.keyboardinputbutton)
        hSizer.AddSpacer(10)
        hSizer.Add(self.logger)
        mainSizer.Add(hSizer, 0, wx.ALL, 5)
        mainSizer.Add(self.button, 0, wx.CENTER)
        
        self.SetSizerAndFit(mainSizer)
        #self.keepKeyboardInputFocus()
        
    def keepKeyboardInputFocus(self, event):
        self.keyboardinputbutton.SetFocus();
    
    def onKeyDown(self, event):
        key = event.GetKeyCode()
        #self.logger.AppendText('KEY DOWN: ' + str(key) + "\n")
        self.keypress[key] = 1
    
    def onKeyUp(self, event):
        key = event.GetKeyCode()
        #self.logger.AppendText('KEY UP: ' + str(key) + "\n")
        self.keypress[key] = 0
    
    def setFocusOverride(self, event):
        self.logger.AppendText('SetFocus' . repr(event))
        
    def OnClick(self,event):
        #self.logger.AppendText(" Click on object with Id %d\n" %event.GetId())
        self.logger.AppendText("keypress " + repr(self.keypress))
    
    def EvtText(self, event):
        self.logger.AppendText('EvtText: %s\n' % event.GetString())
    
    def EvtChar(self, event):
        self.logger.AppendText('EvtChar: %d\n' % event.GetKeyCode())
        event.Skip()
    
    def EvtCheckBox(self, event):
        self.logger.AppendText('EvtCheckBox: %d\n' % event.Checked())     

    def OnPoll(self, event):
    
        keys = self.throttleControl.key_mapping.keys()
        for key in keys:
            key_ord = ord(key)

            if self.keypress.get(key_ord, None) == 1:
                self.throttleControl.key_mapping[key]()
                self.logger.AppendText("throttle: " + repr(self.throttleControl.throttle))
                self.leftThrottle.SetValue(self.throttleControl.throttle['left'])
                self.rightThrottle.SetValue(self.throttleControl.throttle['right'])
                
        
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
        wx.Frame.__init__(self, parent, title=title, size=(720,430))
        self.statusbar = self.CreateStatusBar() # A StatusBar in the bottom of the window
        self.statusbar.PushStatusText("Disconnected... use File -> Connect to connect to the robot")

        # Setting up the menu.
        filemenu= wx.Menu()
        helpmenu= wx.Menu()

        # wx.ID_ABOUT and wx.ID_EXIT are standard ids provided by wxWidgets.
        menuAbout = helpmenu.Append(wx.ID_ABOUT, "&About"," Information about this program")
        menuConnect = filemenu.Append(wx.ID_ANY, "&Connect..."," Open connection to robot")
        menuDisconnect = filemenu.Append(wx.ID_ANY, "&Disconnect"," Disconnect from the robot")
        menuExit = filemenu.Append(wx.ID_EXIT,"&Exit"," Terminate the program")        

        # Creating the menubar.
        menuBar = wx.MenuBar()
        menuBar.Append(filemenu,"&File") # Adding the "filemenu" to the MenuBar
        menuBar.Append(helpmenu,"&Help") # Adding the "filemenu" to the MenuBar
        self.SetMenuBar(menuBar)  # Adding the MenuBar to the Frame content.

        # Set events.
        self.Bind(wx.EVT_MENU, self.OnAbout, menuAbout)
        self.Bind(wx.EVT_MENU, self.OnExit, menuExit)
        self.Bind(wx.EVT_MENU, self.OnConnect, menuConnect)

        self.Show(True)

    def OnAbout(self,e):
        # A message dialog box with an OK button. wx.OK is a standard ID in wxWidgets.
        dlg = wx.MessageDialog( self, 
"""Robot Client Beta
Dan Quinlivan

Begin by connecting to the robot with File -> connect

w - accelerate
s - decelerate
a - turn left
d - turn right

""", "About Robot Client", wx.OK)
        dlg.ShowModal() # Show it
        dlg.Destroy() # finally destroy it when finished.

    def OnExit(self,e):
        self.Close(True)  # Close the frame.
        
    def OnConnect(self,e):
        """ Connect to the robot """
        dlg = wx.TextEntryDialog(
                self, 'Enter <server>:<port> to connect',
                'Open connection to robot', "192.168.0.111:5333")

        if dlg.ShowModal() == wx.ID_OK:
            self.log.WriteText('You entered: %s\n' % dlg.GetValue())

        dlg.Destroy()
            

class ProxyConnection(socketlib.Socket):
    def __init__(self, host, port):
        socketlib.Socket.__init__(self)
        self.create_socket(socket.AF_INET, socket.SOCK_STREAM)
        self.connect( (host, port) )
        self.open = True
        
    def write(self, data):
        socketlib.Socket.write(self, data)
        
                
    def handle_request(self, req):
        if self.user_sock.is_open():
            self.user_sock.write(translated)


            
app = wx.App(False)
frame = MainWindow(None, "6WD Robot Client")
panel = RobotClient(frame, 1)
frame.Show()
app.MainLoop()        