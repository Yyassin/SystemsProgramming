"""
 # Lab 6 - Simple PC Application
 #
 # Python Client Application <uart.py>
 # By: Yousef Yassin June 10, 2021
"""

"""
# **Note** For running the application from the terminal, run python -u applab6.py, adding the -u flag.
# By default Python opens stdin and stdout streams in buffered mode, keeping data
# in memory until a threshold is reached, this breaks the app's console. -u disables the buffering.
"""

# General imports (pip install)
import serial
import serial.tools.list_ports as port_list
from serial.serialutil import SerialException
from enum import Enum

import threading

from tkinter import Tk, Label, StringVar
from tkinter.ttk import Button

""" Enum for sending state update messages to board """
class Tx(Enum):
    NEXT_STATE      = '0'
    PREVIOUS_STATE  = '1'


""" Enum for receiving current state update messages from board """
class State(Enum):
    OFF_STATE       = '0'
    RED_STATE       = '1'
    GREEN_STATE     = '2'
    BOTH_STATE      = '3'
    

""" 
Helper class to manage UART communication
and application label updates. 
"""
class UART:
    # Map from state char code to state name and label colour
    STATE_MAP = {
        State.OFF_STATE:    { "name": "Off State",   "colour": "black" },
        State.RED_STATE:    { "name": "Red State",   "colour": "red" },
        State.GREEN_STATE:  { "name": "Green State", "colour": "green" },
        State.BOTH_STATE:   { "name": "Both State",  "colour": "blue" }
    }

    def __init__(self, baudrate: int, state, state_label, com: str=None):
        """
        Creates a new UART connection instance.

        :param baudrate: int, The connection baudrate.
        :param state: StringVar, The application state tk string variable.
        :param state_label: Label, The application state label.
        :param com: The serial connection com port.
        """
        self._serial          = None        # The serial connection
        self._baudrate        = baudrate    
        self._com             = com         

        self._state           = state       
        self._state_label     = state_label 

    def _get_com(self):
        """ Gets the UART com port if one isn't specified. """
        # The UART port has "UART" in its description:
        # COMX - XDS110 Class Application/User UART (COMX)
        ports = list(port_list.comports())
        for port in ports:
            if "UART" in port.description: 
                self._com = port.name       # COMX
                return;
        exit("Error: UART port could not be found")

    def serial_init(self):
        """ Opens the serial connection. """
        # Get port if not specified
        if self._com is None: self._get_com()

        try:
            # Open connection
            self._serial = serial.Serial(
                port          = self._com,
                baudrate      = self._baudrate,
                bytesize      = serial.EIGHTBITS,    # Number of databits
                timeout       = 2,                   # Read timeout (error if fail for 2ms)
                write_timeout = 2,                   # Write timeout (error if fail for 2ms)
                stopbits      = serial.STOPBITS_ONE  # Number of stopbits
            )
        except SerialException:
            exit(f"Error: Failed to open '{self._com}'")
            
    @staticmethod
    def _prompt():
        """ Prints a prompt arrow for user input in terminal. """
        print(">>> ", end=" ")

    def listen_for_state(self):
        """ 
        Listener thread for incoming UART state update
        messages from the board.
        """
        while True:
            # Wait until a transmission is made
            if self._serial.in_waiting > 0:

                # Put the received byte in serial buffer.
                # use read instead of readline to save time
                # since we know we're receiving max one byte (char).
                serial_buffer = self._serial.read()

                try:
                    # Decode the byte to char and update current state
                    encoded_state = serial_buffer.decode()
                    self._update_state(encoded_state)
                except:
                    print("Failed to decode byte transmission.")
                finally:
                    UART._prompt()

    def _update_state(self, state_code: str):
        """
        Update the internal app state in the terminal
        and application window.

        :param state_code: char, the current state's code.
        """
        # Cast to enum
        state = State(state_code)
        state_msg = f"{self.STATE_MAP[state]['name']} ({state_code})"

        # Print updated state to terminal
        print("\n\nSTATE UPDATED: " + state_msg + "\n")
        # Update application state label (text and colour)
        self._state.set(state_msg)
        self._state_label.config(fg=self.STATE_MAP[state]["colour"])
    
    def _next_state(self):
        """ 
        Send a message to update the board's 
        state machine to the next state. 
        """ 
        self._serial.write(Tx.NEXT_STATE.value.encode())

    def _previous_state(self):
        """ 
        Send a message to update the board's 
        state machine to the previous state. 
        """ 
        self._serial.write(Tx.PREVIOUS_STATE.value.encode())

    def _get_state(self):
        """ 
        Send a message to read the board's 
        current state (without changing the state).
        """ 
        # The encoded message is not in the Tx enum since
        # it's only used at app startup and is "invalid" otherwise.
        GET_STATE = '2' 
        self._serial.write(GET_STATE.encode())
        
    def get_command(self):
        """ 
        Thread for getting user input from
        terminal without blocking other processes.
        """
        while True:
            # Once we get an input, transmit it ou
            self._transmit_char(input())
        
    def _transmit_char(self, command: str):
        """
        Transmits an array of characters through the serial
        port. **Note** This is only used for transmitting chars currently.
        """
        try:
            # Cast the command to Tx enum to make sure it's valid
            state_command = Tx(command)
            if state_command == Tx.NEXT_STATE:
                self._next_state()      # Update to next state message
            elif state_command == Tx.PREVIOUS_STATE:
                self._previous_state()  # Update to previous state message
            # Could be an else since we only have 
            # two enum members but for clarity.

        except ValueError:
            # Enum cast failed
            print("That's an invalid command!")
            UART._prompt()
        

"""
Creates a runnable tkinter application that starts the uart connection,
displays current state and interfaces with the port through state button inputs. 
"""
class Application:
    # The UART baudrate
    BAUDRATE = 115200

    def __init__(self):
        """
        Creates a new application instance.
        """
        # Configure the main window
        self._root = Tk()
        self._root.title("UART App")
        self._root.geometry("250x125")

        # Configure the current state label
        self._current_state = StringVar()
        self._state_label = Label(self._root, textvariable=self._current_state)

        # Open the UART connection, passing references to update the state labels
        self._uart = UART(self.BAUDRATE, self._current_state, self._state_label)
        self._uart.serial_init()
        
    def _setup(self):
        """
        Sets up the application window and background
        threads for UART communication with the board.
        """
        self._init_threads()
        # Get the board's current state. This is needed for 
        # cases where the application is started after the state has already 
        # changed, hence we don't know what it is.
        # The state listener thread will update the label accordingly.
        self._uart._get_state()
        self._init_window()

    def _init_threads(self):
        """
        Sets up asynchronous background threads that listen for
        state update messages from the board and collect user input
        (without blocking) for message transmissions.
        """
        # Make daemon threads so they exit with the main thread.
        t1 = threading.Thread(
            name="input-thread", 
            target=self._uart.get_command, 
            daemon=True
        )  
        t2 = threading.Thread(
            name="listener-thread", 
            target=self._uart.listen_for_state, 
            daemon=True
        )
        t1.start()
        t2.start()

    def _init_window(self):
        """
        Sets up application window with labels and buttons.
        """
        # Create buttons to send next and previous state commands
        next_state_b = Button(self._root, text="Next State", command=self._uart._next_state)
        prev_state_b = Button(self._root, text="Previous State", command=self._uart._previous_state)
        # Label for current state label
        state_label = Label(self._root, text="Current State: ")

        # Configure padding style
        next_state_b.pack(pady=(10,0), ipadx=10)
        prev_state_b.pack(ipadx=6)

        state_label.pack(pady=(10,0))
        self._state_label.pack()


    def run(self):
        """
        Sets up and runs the application.
        """
        self._setup()
        # Keep the main thread running and run the
        # tkinter event loop until the window is closed
        self._root.mainloop()   


if __name__ == "__main__":
    # Application entry point
    Application().run()