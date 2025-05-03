# sfml

## circle-constraint
g++ *.cpp -o game -l sfml-system -l sfml-window -l sfml-graphics -L/opt/homebrew/opt/sfml@2/lib -I/opt/homebrew/opt/sfml@2/include -std=c++17
./game

## box2d-click-boxes
g++ *.cpp -o game -std=c++17 -lsfml-graphics -lsfml-window -lsfml-system -lbox2d -L/opt/homebrew/opt/sfml@2/lib -I/opt/homebrew/opt/sfml@2/include -L/opt/homebrew/opt/box2d/lib -I/opt/homebrew/opt/box2d/include
./game

https://box2d.org/documentation/hello.html