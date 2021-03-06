#!/usr/bin/env python3
#  Copyright 2013 Canonical Ltd.
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU Lesser General Public License as published by
#  the Free Software Foundation; version 3.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.

import time
from menuscript import Script, ActionList, MENU_OBJECT_PATH

al = ActionList(MENU_OBJECT_PATH)
al.appendItem("Menu0", "Menu0")
al.appendItem("Menu1", "Menu1")
al.removeItem(0) # remove Menu0
al.removeItem(0) # remove Menu1

t = Script.create(al)
t.run()

