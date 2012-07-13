import wx
import socketlib
import socket
import asyncore
import pickle
import mysettings
import time

APP_ABOUT = """Robot Client Beta
Dan Quinlivan

Begin by connecting to the robot with File -> connect

w - accelerate
s - decelerate
a - turn left
d - turn right

"""

robotserver = {'host': mysettings.ROBOT_HOST, 'port': 8080}

class RobotPanel(wx.Panel):
    def __init__(self, parent, id):
        
        wx.Panel.__init__(self, parent, id, wx.DefaultPosition,wx.DefaultSize, wx.WANTS_CHARS)
        self.proxy_connection = False
        self.keypress = {}
        self.throttleControl = ThrottleControl()
        self.counter = 0
        self.clock = 0
        
        # window sizers
        mainSizer = wx.BoxSizer(wx.VERTICAL)
        motor_reporting_grid = wx.GridBagSizer(hgap=5, vgap=5)
        motor_grid = wx.BoxSizer(wx.HORIZONTAL)
        robot_reporting_grid = wx.GridBagSizer(hgap=5, vgap=5)
        robot_grid = wx.BoxSizer(wx.VERTICAL)
        button_grid = wx.BoxSizer(wx.VERTICAL)
        cli_grid = wx.BoxSizer(wx.VERTICAL)
        main_grid = wx.BoxSizer(wx.HORIZONTAL)
                
        # create motor fields
        motor_fields = ['RPM(Targ)', 'RPM(Meas)', 'POWER', 'PWM', 'FAULT']
        self.fields = {}
        row = 0
        txtfieldsize = 75
        for field in motor_fields:
            self.fields[field] = {}
            self.fields[field]['name'] = wx.StaticText(self, label=field)
            self.fields[field]['left'] = wx.TextCtrl(self, size=(txtfieldsize,-1), style=wx.TE_READONLY)
            self.fields[field]['right'] = wx.TextCtrl(self, size=(txtfieldsize,-1), style=wx.TE_READONLY)
            motor_reporting_grid.Add(self.fields[field]['name'], pos=(row,1), flag=wx.ALIGN_CENTER )
            motor_reporting_grid.Add(self.fields[field]['left'], pos=(row,0))
            motor_reporting_grid.Add(self.fields[field]['right'], pos=(row,2))
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
        
        # Robot Reporting Grid
        
        row = 0
        robot_fields = ['BATT VOLTS', 'LOAD', 'BATT TEMP', 'CPU TMP', 'DRIVER TEMP']
        for field in robot_fields:
            self.fields[field] = {}
            self.fields[field]['name'] = wx.StaticText(self, label=field)
            self.fields[field]['value'] = wx.TextCtrl(self, size=(txtfieldsize,-1), style=wx.TE_READONLY)
            robot_reporting_grid.Add(self.fields[field]['name'], pos=(row, 0), flag=wx.ALIGN_CENTER_VERTICAL )
            robot_reporting_grid.Add(self.fields[field]['value'], pos=(row, 1))
            #wx.EVT_SET_FOCUS(self.fields[field]['name'], self.keepKeyboardInputFocus)
            wx.EVT_SET_FOCUS(self.fields[field]['value'], self.keepKeyboardInputFocus)
            row += 1
             
        
        
        
        # Keyboard Input
        self.keyboardinputbutton = wx.Button(self, label="Keyboard Input")        
        wx.EVT_KEY_DOWN(self.keyboardinputbutton, self.onKeyDown)    
        wx.EVT_KEY_UP(self.keyboardinputbutton, self.onKeyUp)    
        self.keyboardinputbutton.SetFocus();
        
        # CPU Shutdown button
        self.cpushutdownbutton = wx.Button(self, label="CPU Shutdown")
        self.Bind(wx.EVT_BUTTON, self.OnCPUShutdown,self.cpushutdownbutton)
        
        # Logging panel
        self.logger = wx.TextCtrl(self, size=(300,300), style=wx.TE_MULTILINE | wx.TE_READONLY)
        wx.EVT_SET_FOCUS(self.logger, self.keepKeyboardInputFocus)
        
        # CLI interface
        self.cli = wx.StaticText(self, label="Command Line Interface")
        self.cli_field = wx.TextCtrl(self, size=(300,-1), style = wx.TE_PROCESS_ENTER)
        self.Bind(wx.EVT_TEXT_ENTER, self.OnCLISubmit, self.cli_field)


        # wxTimer events
        self.poller = wx.Timer(self, wx.NewId())
        self.Bind(wx.EVT_TIMER, self.OnPoll)
        self.poller.Start(100, wx.TIMER_CONTINUOUS)
    
        
        # arrange widgets
        motor_grid.Add(self.leftThrottle)
        motor_grid.Add(motor_reporting_grid, 0, wx.ALL, 5)
        motor_grid.Add(self.rightThrottle)
        robot_grid.Add(motor_grid)
        robot_grid.Add(robot_reporting_grid, flag=wx.ALIGN_CENTER)
        button_grid.Add(self.keyboardinputbutton)
        button_grid.Add(self.cpushutdownbutton)
        cli_grid.Add(self.logger)
        cli_grid.AddSpacer(3)
        cli_grid.Add(self.cli)
        cli_grid.Add(self.cli_field)
        main_grid.Add(robot_grid)
        main_grid.Add(button_grid)
        main_grid.AddSpacer(10)
        main_grid.Add(cli_grid)
        mainSizer.Add(main_grid, 0, wx.ALL, 5)
        
        self.SetSizerAndFit(mainSizer)
        
    def keepKeyboardInputFocus(self, event):
        self.keyboardinputbutton.SetFocus();
    
    def onKeyDown(self, event):
        key = event.GetKeyCode()
        self.keypress[key] = 1
    
    def onKeyUp(self, event):
        key = event.GetKeyCode()
        if key == wx.WXK_RETURN:
            self.cli_field.SetFocus()
            return
        self.keypress[key] = 0
        
    def OnCLISubmit(self, event):
        self.keyboardinputbutton.SetFocus();
        if self.proxy_connection == False:
            self.logger.AppendText("Not connected to Robot Server, connect first")
            return
        self.logger.AppendText("CMD Sent: " + str(self.cli_field.GetValue()) + "\n")
        self.proxy_connection.write(str(self.cli_field.GetValue()))
        self.cli_field.Clear()
        
    def OnCPUShutdown(self, event):
        self.proxy_connection.write(str("cpu_shutdown"))
        self.keyboardinputbutton.SetFocus();
        
    
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
                if time.clock() - self.clock < 5: return
                self.clock = time.clock()                
                self.proxy_connection = ProxyConnection(robotserver['host'], int(robotserver['port']))  
                                
            if self.proxy_connection.is_open() == True:
                # send and receive socket data
                self.proxy_connection.write(
                    "rpmset," + 
                    str(int(self.throttleControl.throttle['left']) * 100) + "," + 
                    str(int(self.throttleControl.throttle['right']) * 100))
                
                self.counter += 1
                if self.counter < 5: return 
                self.proxy_connection.write('get_status')
                self.counter = 0
                
    def UpdateFields(self, fields):
        if len(fields) < 1: return
        self.fields['RPM(Meas)']['left'].SetValue(format_RPM(fields['L'][0]))
        self.fields['POWER']['left'].SetValue(format_Watts(fields['L'][1]))
        self.fields['PWM']['left'].SetValue(format_PWM(fields['L'][2]))
        
        self.fields['RPM(Meas)']['right'].SetValue(format_RPM(fields['R'][0]))
        self.fields['POWER']['right'].SetValue(format_Watts(fields['R'][1]))
        self.fields['PWM']['right'].SetValue(format_PWM(fields['R'][2]))
        
        
        self.fields['RPM(Targ)']['left'].SetValue(format_RPM(abs(int(self.leftThrottle.GetValue()) * 100)))
        self.fields['RPM(Targ)']['right'].SetValue(format_RPM(abs(int(self.rightThrottle.GetValue()) * 100)))
        self.fields['BATT VOLTS']['value'].SetValue(format_voltage(fields['M'][0]))
        self.fields['LOAD']['value'].SetValue(format_Watts(int(fields['R'][1]) + int(fields['L'][1]) + 12600))
 
def format_Watts(val):
    watts = float(val) / 1000
    watts = '{0:0>4.1f} W'.format(watts)
    return watts
    
def format_RPM(val):
    return '{0:0>5d} '.format(int(val))
    
def format_PWM(val):
    pwm = float(val) * (100.0/1024.0)
    pwm = str(int(pwm)) + "%"
    return pwm
    
def format_voltage(val):
    volts = float(val)
    volts /= 1000
    return '{0:0>3.2f} V'.format(volts)
    
 
class ThrottleControl:
    
    def __init__(self):
        self.key_mapping = {'A': self.turnleft, 
                            'S': self.center,
                            'D': self.turnright,
                            'I': self.accelerate, 
                            'K': self.sit,
                            ',': self.decelerate,
                            ' ': self.fullstop
                            }
        self.THROTTLE_INCREMENT = 2
        self.throttle = {'left': 0.0, 'right': 0.0}
    def turnleft(self):
        self.throttle['left'] -= self.THROTTLE_INCREMENT
        self.throttle['right'] += self.THROTTLE_INCREMENT
        self.validate()    
    def center(self):
        center = (self.throttle['left'] + self.throttle['right']) / 2
        self.throttle['left'] = center
        self.throttle['right'] = center     
        self.validate()    
    def turnright(self):
        self.throttle['left'] += self.THROTTLE_INCREMENT
        self.throttle['right'] -= self.THROTTLE_INCREMENT
        self.validate()   
    def accelerate(self):
        self.throttle['left'] += self.THROTTLE_INCREMENT
        self.throttle['right'] += self.THROTTLE_INCREMENT
        self.validate()
    def sit(self):
        center = (self.throttle['left'] + self.throttle['right']) / 2
        self.throttle['left'] -= center
        self.throttle['right'] -= center               
    def decelerate(self):
        self.throttle['left'] -= self.THROTTLE_INCREMENT
        self.throttle['right'] -= self.THROTTLE_INCREMENT
        self.validate()
    def fullstop(self):
        self.throttle['left'] = 0
        self.throttle['right'] = 0
        
        
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
        if req.find('robot_status') != -1: 
            fields = req[12:]
            return robot_panel.UpdateFields(pickle.loads(fields))
        robot_panel.logger.AppendText(req + "\n")
        
    def handle_close(self):
        robot_panel.logger.AppendText("Not connected to Robot Server\n")
        socketlib.Socket.handle_close(self)
        
    def handle_connect(self):
        robot_panel.logger.AppendText("Connection established!\n")
    



        
app = wx.App(False)
frame = MainWindow(None, "6WD Robot Client")
robot_panel = RobotPanel(frame, 1)
frame.Show()
app.MainLoop()        
