import bpy
import numpy as np
import os

texture_nodes = []
obj = bpy.context.active_object 
if obj.active_material is None:
    mat = bpy.data.materials.new(name="BakeMaterial")
    obj.data.materials.append(mat) 

for mat in obj.data.materials:
    if not mat.use_nodes:
        mat.use_nodes = True
    nodes = mat.node_tree.nodes
    if 'BakeTextureNode' not in nodes:
        texture_node = nodes.new(type='ShaderNodeTexImage')
        texture_node.name = 'BakeTextureNode'
    else:
        texture_node = nodes['BakeTextureNode']
    nodes.active = texture_node
    texture_nodes.append(texture_node)
    
bpy.context.view_layer.objects.active = obj
obj.select_set(True)


active_object_name = bpy.context.active_object.name
file_name = os.path.splitext(bpy.path.basename(bpy.data.filepath))[0]
TexturePrefix = active_object_name+"_"+file_name
Res = 1024

imagec = bpy.data.images.new(name=TexturePrefix+"_c", width=Res, height=Res,is_data=False)
imagen = bpy.data.images.new(name=TexturePrefix+"_n", width=Res, height=Res,is_data=True)
imagem = bpy.data.images.new(name=TexturePrefix+"_m", width=Res, height=Res,is_data=True)
imagef = bpy.data.images.new(name=TexturePrefix+"_fx", width=Res, height=Res,is_data=True)
ROUGHNESS = bpy.data.images.new(name=TexturePrefix+"_rough", width=Res, height=Res,is_data=True)
AO = bpy.data.images.new(name=TexturePrefix+"_ao", width=Res, height=Res,is_data=True)
METAL = bpy.data.images.new(name=TexturePrefix+"_meta", width=Res, height=Res,is_data=True)

def bake_texture(texture_type, image, pass_filter, file_name):
    for texNodes in texture_nodes:
        texNodes.image = image        
    bpy.ops.object.bake(type=texture_type, pass_filter=pass_filter, use_clear=True) 
    image.filepath_raw = file_name
    image.file_format = 'PNG'
    image.save()
    print("Baking completed and saved to " + file_name)

tasks = [
    ('DIFFUSE', imagec, {'COLOR'}, "//"+TexturePrefix+"_c.png"),
    ('NORMAL', imagen, {'NONE'}, "//"+TexturePrefix+"_n.png"),
    ('EMIT', imagef, {'NONE'}, "//"+TexturePrefix+"_fx.png"),
    ('ROUGHNESS', ROUGHNESS, {'NONE'}, "//"+TexturePrefix+"_rough.png"),
    ('AO', AO, {'NONE'}, "//"+TexturePrefix+"_ao.png"),
    ('GLOSSY', METAL, {'COLOR'}, "//"+TexturePrefix+"_meta.png")  # no metallic in blender ??
]

for texture_type, image, pass_filter, file_name  in tasks:
    bake_texture(texture_type, image, pass_filter, file_name)

w, h = imagem.size
outMaterial = np.zeros((w * h * 4,), dtype='float32')
rough = np.array(ROUGHNESS.pixels[:])
ao = np.array(AO.pixels[:])
metal = np.array(METAL.pixels[:])

outMaterial[0::4] = ao[0::4]
outMaterial[1::4] = rough[1::4]
outMaterial[2::4] = metal[2::4]
outMaterial[3::4] = 1.0

imagem.pixels.foreach_set(outMaterial.ravel()) 
imagem.filepath_raw = "//"+TexturePrefix+"_m.png"
imagem.file_format = 'PNG'
imagem.save()

print("Baking completed")
print("Duplicating mesh")


bpy.ops.object.duplicate()
new_obj = bpy.context.active_object

new_obj.data.materials.clear()


new_mat = bpy.data.materials.new(name="CombinedBakedMaterial")
new_obj.data.materials.append(new_mat)

new_mat.use_nodes = True
new_nodes = new_mat.node_tree.nodes
new_links = new_mat.node_tree.links

for node in new_nodes:
    new_nodes.remove(node)
    
output_node = new_nodes.new(type='ShaderNodeOutputMaterial')
principled_node = new_nodes.new(type='ShaderNodeBsdfPrincipled')
diffuse_node = new_nodes.new(type='ShaderNodeTexImage')
normal_node = new_nodes.new(type='ShaderNodeTexImage')
metal_rough_ao_node = new_nodes.new(type='ShaderNodeTexImage')
separate_rgb_node = new_nodes.new(type='ShaderNodeSeparateRGB')

diffuse_node.image = imagec
normal_node.image = imagen
metal_rough_ao_node.image = imagem



new_links.new(diffuse_node.outputs['Color'], principled_node.inputs['Base Color'])
new_links.new(normal_node.outputs['Color'], principled_node.inputs['Normal'])  

new_links.new(metal_rough_ao_node.outputs['Color'], separate_rgb_node.inputs['Image']) 
new_links.new(separate_rgb_node.outputs['G'], principled_node.inputs['Roughness'])
new_links.new(separate_rgb_node.outputs['B'], principled_node.inputs['Metallic']) 


new_links.new(principled_node.outputs['BSDF'], output_node.inputs['Surface'])

normal_map_node = new_nodes.new(type='ShaderNodeNormalMap')
new_links.new(normal_node.outputs['Color'], normal_map_node.inputs['Color'])
new_links.new(normal_map_node.outputs['Normal'], principled_node.inputs['Normal'])

print("Baking and material setup completed and saved to //baked_texture.png")