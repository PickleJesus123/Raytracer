# Raycaster-Project
My C++ Raycaster, that supports diffuse lighting, hard shadows, triangle meshes, and reflective surfaces.
 
###ABOUT 
- This raycaster renders a simple scene with hard shadows, and diffuse lighting 
- The scene is made of up of sphere, triangle, and quad primitives. 
- Quads are constructed via 2 triangles. 
- Rays are emitted from every pixel in the screen in raycastScene() from the view position. 
- The rays check every object in the scene to see if a collision occurs. 
- If a collision occurs, several things occur 
 * Diffuse lighting : depending on the angle of the normal that is calculated, a shading value is assigned to that pixel, depending on the intensity of the light source.
 * Hard shadows : from the point of intersection, the ray checks whether or not it can see every light added to the scene. If it can see the light, it gains light value depending on the intensity of the light. 
  * Reflections : If reflections is enabled on the object, the previous two steps are ignored, and a new ray is cast from the point of intersection, and lighting is calculated from that ray. Rays only bounce once, currently. 
  * Light orbs : Light orbs are a special case, in which shading is not rendered. 
