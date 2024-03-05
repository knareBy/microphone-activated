#include <Windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_set>
#include <vector>
#include <mmsystem.h>
#include <chrono>
#include <thread>
#pragma comment(lib, "Winmm.lib")

// Function to check if a key is pressed
bool IsKeyPressed(int keyCode) {
    return GetAsyncKeyState(keyCode) & 0x8000;
}

// Function to get the key combination from the user
std::vector<int> GetKeyCombination() {
    std::vector<int> keys;
    std::unordered_set<int> pressedKeys;
    std::cout << "Press the key(s) you want to use as the key combination. Press ENTER to finish." << std::endl;
    auto startTime = std::chrono::steady_clock::now();
    while (true) {
        for (int i = 0; i < 256; ++i) {
            if (IsKeyPressed(i) && pressedKeys.find(i) == pressedKeys.end()) {
                if (i == VK_RETURN) break;
                keys.push_back(i);
                pressedKeys.insert(i);
                // std::cout << "Key pressed: " << i << std::endl;
            }
        }
        if (IsKeyPressed(VK_RETURN)) {
            auto currentTime = std::chrono::steady_clock::now();
            if (std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime).count() >= 3) break;
        }
    }   
    return keys;
}

int main() {
    bool microphone_activated = false;
    bool prev_state = true;
    
    // Check if keybind.txt exists
    std::ifstream keybindFile("keybind.txt");
    std::vector<int> keyCombination;
    if (keybindFile.good()) {
        int keyCode;
        while (keybindFile >> keyCode)
            keyCombination.push_back(keyCode);
        std::cout << "Key combination loaded from keybind.txt" << std::endl;
        std::cout << "If you want to change the key combination, delete keybind.txt and restart the program." << std::endl;
    } else {
        keyCombination = GetKeyCombination();
        
        // Save the key combination to keybind.txt
        std::ofstream outFile("keybind.txt");
        if (outFile.is_open()) {
            for (int keyCode : keyCombination)
                outFile << keyCode << " ";
            outFile.close();
            std::cout << "Saved key combination to keybind.txt." << std::endl;
            std::cout << "If you want to change the key combination, delete keybind.txt and restart the program." << std::endl;
        } else {
            std::cerr << "Unable to open keybind.txt for writing" << std::endl;
            return 1;
        }
    }

    while (true) {
        bool allPressed = true;
        for (int keyCode : keyCombination) {
            if (!IsKeyPressed(keyCode)) {
                allPressed = false;
                break;
            }
        }

        if (allPressed) {
            if (!prev_state) {
                microphone_activated = true;
                std::cout << "Microphone activated" << std::endl;
                PlaySoundA("microphone_activated.wav", NULL, SND_FILENAME | SND_ASYNC);
                prev_state = microphone_activated;
            } else {
                microphone_activated = false;
                std::cout << "Microphone muted" << std::endl;
                PlaySoundA("microphone_muted.wav", NULL, SND_FILENAME | SND_ASYNC);
                prev_state = microphone_activated;
            }

            while (allPressed) {
                allPressed = true;
                for (int keyCode : keyCombination) {
                    if (!IsKeyPressed(keyCode)) {
                        allPressed = false;
                        break;
                    }
                }
            }
        }
	Sleep(100);
    }
    
    return 0;
}
