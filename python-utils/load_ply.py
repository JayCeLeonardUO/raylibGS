import bpy

# Delete all existing objects
bpy.ops.object.select_all(action='SELECT')
bpy.ops.object.delete()

# Import the PLY file
bpy.ops.wm.ply_import(filepath="output.ply", 
                      directory=".", 
                      files=[{"name":"output.ply", "name":"output.ply"}])