# From Chatgpt

import bpy
import sys
import os

# --- DEFAULT SETTINGS ---
default_collection_name = "Main"
default_output_path = bpy.path.abspath("//render_output.png")

# --- PARSE CUSTOM ARGUMENTS ---
argv = sys.argv
if "--" in argv:
    argv = argv[argv.index("--") + 1:]  # everything after '--' belongs to us
else:
    argv = []

collection_name = default_collection_name
output_path = default_output_path

for arg in argv:
    if arg.startswith("output="):
        output_path = os.path.abspath(arg.split("=", 1)[1])
    elif arg.startswith("collection="):
        collection_name = arg.split("=", 1)[1]

# --- FUNCTIONS ---

def set_render_visibility(collection, enable):
    """Recursively set render visibility for a collection and its children."""
    collection.hide_render = not enable
    for child in collection.children:
        set_render_visibility(child, enable)

def find_collection(name, parent=None):
    """Recursively search for a collection by name."""
    collections = bpy.data.collections if parent is None else parent.children
    for c in collections:
        if c.name == name:
            return c
        found = find_collection(name, c)
        if found:
            return found
    return None

# --- MAIN ---

target_col = find_collection(collection_name)
if not target_col:
    raise ValueError(f"Collection '{collection_name}' not found in this file!")

# Disable render for all collections
for col in bpy.data.collections:
    col.hide_render = True

# Enable render for the chosen collection and its children
set_render_visibility(target_col, True)

# Render the scene
bpy.context.scene.render.filepath = output_path
bpy.ops.render.render(write_still=True)

print(f"✅ Render complete.")
print(f"   Collection rendered: {collection_name}")
print(f"   Output saved to: {output_path}")
