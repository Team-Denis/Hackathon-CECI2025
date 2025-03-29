#pragma once

#include "common.hpp"
#include <qrencode.h>
#include <raylib.h>


const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 800;

void draw_qr(std::string filename);