# zobjExporter
Export a display list into its own zobj file

Purpose: to more easily throw a display list from one object into another object file to mess around with in Hylian Toolbox. You can use something like ZOBJrecalcV2 to get it working in other object files.

Usage

Give it a zobj file

Give the display list you want to export

Give it the texture length for each offset. You'll have to find this manually (I have a program by I think flotonic called textdoc.exe that does this in terminal)

It should output a file called "newZobj.zobj" in the same directory

Format of the output is:

Display list data (that points to vector and texture data)

Vector data

Texture data


