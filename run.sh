rm -rf build && cmake -S . -B build && cmake --build build && \

hyperfine --runs 5 './build/GameEngine & sleep 1 && ./build/Game & ./build/Game & ./build/Game' --export-json string_moving_1000.json