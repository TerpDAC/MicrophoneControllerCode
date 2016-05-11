from MicSense.db import db, initDatabase
from MicSense.models import Data

import sys

def confirm():
    while 1:
        yn = input("Confirm? [y/n] ")
        if yn and len(yn) > 0:
            if yn.lower()[0] == "y":
                return True
            elif yn.lower()[0] == "n":
                return False
            else:
                print("Invalid option!")

def intinput(field):
    while 1:
        field_in = input("%s: " % field)

        if not field_in.isdigit():
            print("Invalid %s - you need to specify a number!")
        else:
            return int(field_in)

def select(field, options):
    while 1:
        print("For %s, select from the following:" % field)
        print("=============================================")
        cur = 1
        for opt in options:
            print(" %i) %s" % (cur, opt[1]))
            cur += 1
        choice = intinput(field)

        if (choice > 0) and (choice <= len(options)):
            print("You selected choice %i: %s." % (choice, options[choice - 1][1]))
            if confirm(): break
        else:
            print("Invalid choice!")

    return options[choice - 1]

def deleteAllData():
    while 1:
        print("Confirm clearing all data!")
        print("==========================")

        if confirm(): break

    Data.query.delete()
    db.session.commit()

def listData():
    data = Data.query.all()

    print("Data:")
    print("======")
    for e in data:
        print(" * %s | High: %i | Med: %i | Low: %i" % (str(e.timestamp), e.high, e.med, e.low))
    print("")

def listLatestData():
    e = Data.query.order_by('-timestamp').first()

    print("Latest Data:")
    print("============")
    print(" * %s | High: %i | Med: %i | Low: %i" % (str(e.timestamp), e.high, e.med, e.low))

def main():
    while 1:
        menu_opts = [
                        [listData,           "List Data"],
                        [listLatestData,     "List Latest Data"],
                        [deleteAllData,      "Delete All Data"],
                        [sys.exit,           "Exit"],
                    ]
        menu_pick = select("Command", menu_opts)
        print("Selected: %s" % menu_pick[1])
        menu_pick[0]()

if __name__ == "__main__":
    main()