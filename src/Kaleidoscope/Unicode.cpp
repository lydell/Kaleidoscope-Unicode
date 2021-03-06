/* -*- mode: c++ -*-
 * Kaleidoscope-Unicode -- Unicode input helpers
 * Copyright (C) 2016, 2017  Gergely Nagy
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <Kaleidoscope-Unicode.h>
#include "kaleidoscope/hid.h"

namespace kaleidoscope {

Unicode::Unicode(void) {
}

void Unicode::begin(void) {
  ::HostOS.begin();
}

void Unicode::start(void) {
  switch (::HostOS.os()) {
  case hostos::LINUX:
    hid::pressRawKey(Key_LeftControl);
    hid::pressRawKey(Key_LeftShift);
    hid::pressRawKey(Key_U);
    hid::sendKeyboardReport();
    hid::releaseRawKey(Key_LeftControl);
    hid::releaseRawKey(Key_LeftShift);
    hid::releaseRawKey(Key_U);
    hid::sendKeyboardReport();
    break;
  case hostos::WINDOWS:
    hid::pressRawKey(Key_RightAlt);
    hid::sendKeyboardReport();
    hid::releaseRawKey(Key_RightAlt);
    hid::sendKeyboardReport();
    hid::pressRawKey(Key_U);
    hid::sendKeyboardReport();
    hid::releaseRawKey(Key_U);
    hid::sendKeyboardReport();
    break;
  case hostos::OSX:
    hid::pressRawKey(Key_LeftAlt);
    break;
  default:
    unicodeCustomStart();
    break;
  }
}

void Unicode::input(void) {
  switch (::HostOS.os()) {
  case hostos::LINUX:
  case hostos::WINDOWS:
    break;
  case hostos::OSX:
    hid::pressRawKey(Key_LeftAlt);
    break;
  default:
    unicodeCustomInput();
    break;
  }
}

void Unicode::end(void) {
  switch (::HostOS.os()) {
  case hostos::LINUX:
    hid::pressRawKey(Key_Spacebar);
    hid::sendKeyboardReport();
    hid::releaseRawKey(Key_Spacebar);
    hid::sendKeyboardReport();
    break;
  case hostos::WINDOWS:
    break;
  case hostos::OSX:
    hid::releaseRawKey(Key_LeftAlt);
    hid::sendKeyboardReport();
    break;
  default:
    unicodeCustomEnd();
    break;
  }
}

void Unicode::typeCode(uint32_t unicode) {
  bool on_zero_start = true;
  for (int8_t i = 7; i >= 0; i--) {
    if (i <= 3) {
      on_zero_start = false;
    }
    uint8_t digit = ((unicode >> (i * 4)) & 0xF);
    if (digit == 0) {
      if (on_zero_start == false) {
        Key key = hexToKey(digit);
        input();
        hid::pressRawKey(key);
        hid::sendKeyboardReport();
        input();
        hid::releaseRawKey(key);
        hid::sendKeyboardReport();
      }
    } else {
      Key key = hexToKey(digit);
      input();
      hid::pressRawKey(key);
      hid::sendKeyboardReport();
      input();
      hid::releaseRawKey(key);
      hid::sendKeyboardReport();
      on_zero_start = false;
    }
    delay(5);
  }
}

void Unicode::type(uint32_t unicode) {
  start();
  typeCode(unicode);
  end();
}

}

__attribute__((weak)) Key hexToKey(uint8_t hex) {
  uint8_t m;
  if (hex == 0x0) {
    return Key_0;
  }
  if (hex < 0xA) {
    m = Key_1.keyCode + (hex - 0x1);
  } else {
    m = Key_A.keyCode + (hex - 0xA);
  }
  return { m, KEY_FLAGS };
}

__attribute__((weak)) void unicodeCustomStart(void) {
}

__attribute__((weak)) void unicodeCustomEnd(void) {
}

__attribute__((weak)) void unicodeCustomInput(void) {
}

kaleidoscope::Unicode Unicode;
