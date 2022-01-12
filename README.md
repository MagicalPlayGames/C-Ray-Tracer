# C-Ray-Tracer
This is a c++ Ray Tracer I made myself, but works under specific conditions

.ppm file:
  Each input .ppm file must be in P3 format in the following format:
  xSize ySize
  x,y: color  Hexcode  Color/srgb(187,135,107)
  ....
  512 512
  1,0: (255,255,255)  #FFFFFF  white
  ....
  
  The output .ppm call out.pom file is in P3 format in the following format:
  P3
  # Raytracer out.ppm
  512 512 255  (xSize, ySize, color type(255))
  102 102 102  (pixel color x,y,z)
  102 102 102  (pixel color x,y,z)
  ...
  
  It is recommended to open out.ppm in giphy

.scn file:
  Each render must have a .scn file that transforms objects, uses .obj files, and .ppm files as texutres and normal maps
  
  The commands are as follow:
  view screenSize screenWidth - Sets the view of the render
  # - a comment line
  Object name.obj - loads name.obj as a object
  scale x y z - scales the next objects by x y z
  move x y z - moves the next objects by x y z
  rotate angle x y z - rotates the next objects by x y z using angle
  light r g b x y z - creates a light at position x y z with color r g b
  background r g b - sets the background color
  ambient r g b - sets ambient lighting
  group - starts a group to contain transformations
  groupend - ends a group reverting the transformations to before the start of a group
  material dr dg db sr sg sb shininess - sets the material variables to diffuse (dr,dg,db), specular (sr,sg,sb), and shininess
  
  
  .obj file:
    Typical .obj  files used are files exported from blender

