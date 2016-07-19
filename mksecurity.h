#pragma once
#include <windows.h>
#include <winioctl.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <iphlpapi.h>
using namespace std;

static const wchar_t regKey[] = L"Software\\mediamethod\\fft";

uint32_t GetVolume();
uint32_t GetMAC();
uint32_t key_encode(uint32_t seed); 
uint32_t GetSeed();
uint32_t GetKey    (uint32_t seed);
bool     RegisterValify();