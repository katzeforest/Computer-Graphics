How To Run
  Put all the test files in the debug default folder. Open HW1a.sln in VS 2010.  Right click the Solution and go to Properties -> Configuration Properties -> Debugging -> Command Arguments to change different test files. Choose release to run the project. My scene is the snowman.txt. I did not calculate the center of the arms or nose in an accurate number so there maybe some deviation when do keyboard transformation.


Homework # 2a

Requirements:

1. Scene Graph
    It is in scene.cpp, scene.h, node.cpp and node.h. Transformations methods are in void Node::drawScene.

2. Geometry
    It is in cube.cpp and cube.h. The drawing is in void Node::drawScene.

3. File Input
    Please check config.h and config.cpp. The changes are based on HW1.
4. Interactivity
    All in void keypress(unsigned char key, int x, int y).

Extra Credit:
1. Efficient VBOs
    Data is uploaded only when the scene changes.
2. Delete a Node
    It is in the case 'u' in void keypress(unsigned char key, int x, int y).
