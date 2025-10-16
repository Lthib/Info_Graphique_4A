#include <texturing/CubeMapUtils.hpp>
#include <GL/glew.h>

/**
* When using environment lighting, blurring the environnment texture/cubemap
* is a good way to simulate glossy reflections and even diffuse lighting.
*
* Blurring a cubemap is tricky because we cannot blur the face seperately 
* else we will clearly see the boundaries between them.
* We need to perform the low pass filter in the spherical domain.
* As an approximation of spherical domain, we use a panomara which is 
* basically a (H,W,4) image where each pixel corresponds to a direction
* represented two angles phi and theta. Horizontally the pixels correspond 
* to phi in [0, 2PI] and vertically to theta in [0, PI].
*
* We manage to convert an input cubemap into this panorama format and perform a 
* gaussian blur (vertically, then horizontally).
* The panorama is eventually converted back to cubemap faces.
*
* Note : All the operations are performed using OpenGL and shaders.
*        Since they run on the GPU, they are really fast.
**/

int main()
{
    // The functions for blurring a cubemap use OpenGL and SFML functionalities.
    // We need to initizalize OpenGL's context and a sf::Window.
    sf::RenderWindow window(sf::VideoMode(0, 0), "dummy window");
    window.setVisible(false);
    glewInit();

    // Input directory, must exist
    std::string skybox_dir = "../../sfmlGraphicsPipeline/textures/skybox";
    // Output directory, must exist
    std::string skybox_blurred_dir = "../../sfmlGraphicsPipeline/textures/skybox/diffuse";
    // The height (and width) of each output cubemap face images 
    unsigned int csize = 256u;
    // The gaussian kernel size (sigma = (ksize + 1) / 6)
    // The gaussian blur is performed on RESIZED face images.
    unsigned int ksize = 512u;

    cmutils::blur_cubemap_directory(skybox_dir, skybox_blurred_dir, csize, ksize);

    skybox_blurred_dir = "../../sfmlGraphicsPipeline/textures/skybox/glossy";
    csize = 512u;
    ksize = 100u;

    cmutils::blur_cubemap_directory(skybox_dir, skybox_blurred_dir, csize, ksize);
}
