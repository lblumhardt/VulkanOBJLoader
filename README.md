# Vulkan-engine

This is a Vulkan C++ project that currently loads OBJ files and displays then in a GLFW window rotating around its y-axis

TODO:
- Fix validation error related to image format in call to VkCreateImageView()
- Extract utility functions out of Texture.cpp and Model.cpp
- Fix memory freeing issue in CommandPool.cpp

Next features to work on:
- Mouse camera control (with zoom)
- All user to change current model/texture with string input
