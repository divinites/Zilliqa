#!/bin/bash
# Copyright (C) 2022 Zilliqa
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

success=0
while [[ "${success}" == "0" ]]
do
   export TESTSEED=$RANDOM;
   run=0
   while [[ "${success}" == 0 && "${run}" -lt 5 ]]
   do
     pytest -s -v --log-level=DEBUG
     success="$?"
     run=$((run+1))
   done
done
