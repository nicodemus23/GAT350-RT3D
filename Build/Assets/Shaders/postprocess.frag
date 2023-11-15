#version 430

#define INVERT_MASK				 (1 << 0)
#define GRAYSCALE_MASK			 (1 << 1)
#define	COLORTINT_MASK			 (1 << 2)
#define	SCANLINE_MASK			 (1 << 3)
#define	GRAIN_MASK		         (1 << 4)
#define	BLUR_MASK				 (1 << 5)
#define	BLUR5x5_MASK			 (1 << 6)
#define	RADIAL_BLUR_MASK	     (1 << 7)
#define	EDGE_DETECTION_MASK		 (1 << 8)

in layout(location = 0) vec2 ftexcoord;
out layout(location = 0) vec4 ocolor; // this is the pixel we draw to the screen 

uniform float blend = 1;
uniform uint params = 0;
uniform vec3 tintRGB;

uniform float scanlineFrequency = 50.0; // number of scanlines 
uniform float scanlineIntensity = 0.5; // 0.0 to 1.0

uniform float grainIntensity = 0.5;
uniform float time;

// edge detection:
uniform float edgeIntensity = 0.0;

// Radial Gaussian Blur settings:
uniform float uBlurDirections; // 8.0
uniform float uBlurQuality; // 2.0
uniform float uBlurSize; // 8.0
uniform float radialBlurIntensity = 0.0;

uniform int textureWidth;
uniform int textureHeight;

// blurKernel approach:
uniform vec2 texelSize;
uniform float blurIntensity;

// for radial blur:
float luminance(vec3 color) 
    {
        return dot(color, vec3(0.299, 0.587, 0.114));
    }
// keeps borders from bleeding in RadialBlur at high sampling size:
vec2 handleBorders(vec2 uv) 
{
    uv = fract(uv); // Wrap the coordinates to keep them within [0, 1]
    uv = mix(uv, 1.0 - uv, step(1.0, uv * 2.0)); // Reflect at borders
    return uv;
}

// define kernel size for 3 x 3 gaussian blur:
const float blurKernel[9] = float[](
	1.0/16, 2.0/16, 1.0/16,
	2.0/16, 4.0/16, 2.0/16,
	1.0/16, 2.0/16, 1.0/16
);
const int blurKernelSize = 3;

// define kernel size for 5 x 5 gaussian blur:
const float blur5x5Kernel[25] = float[](
    1.0/273, 4.0/273, 7.0/273, 4.0/273, 1.0/273,
    4.0/273,16.0/273,26.0/273,16.0/273, 4.0/273,
    7.0/273,26.0/273,41.0/273,26.0/273, 7.0/273,
    4.0/273,16.0/273,26.0/273,16.0/273, 4.0/273,
    1.0/273, 4.0/273, 7.0/273, 4.0/273, 1.0/273
	);
const int blur5x5KernelSize = 5;

// define edge detection kernel size: 
const float edgeKernel[9] = float[](
	-1, -1, -1, 
	-1,  8, -1,
	-1, -1, -1
);
const int edgeKernelSize = 3;

// this is bound to channel 0
layout(binding = 0) uniform sampler2D screenTexture;

vec4 invert(in vec4 color)
{ // extract rgb components from input color (color.rgb), take original vec3(1) (white) and subtract color from max value (1) gives opposite color, inverted color (rgb) and alpha then combined back into 4 component vector (vec4)
return  vec4(vec3(1) - color.rgb, color.a);
}

vec4 grayscale(in vec4 color)
{ // rgb/3 calculates luminance by averaging red/green/blue of input color
return vec4(vec3((color.r + color.b + color.g) / 3), color.a);
}

vec4 colorTint(in vec4 color, in vec3 tintRGB)
{
 return vec4(vec3(color.rgb * tintRGB.rgb), color.a);
}

vec4 scanline(in vec4 color, in vec2 uv, float frequency, float intensity)
{
	float scanlineFX = 1.0 - abs(sin(uv.y * frequency * 3.14159)) * intensity;

	return vec4(color.rgb * scanlineFX, color.a);
}
vec4 grain(in vec2 uv, in vec4 sceneColor, float time, float intensity) {
    float noise = fract(sin(dot(uv + time, vec2(12.9989, 78.2333))) * 45876.5453);
    float grainValue = noise * 2.0 - 1.0; // grain value range [-1, 1]
    vec3 grainColor = sceneColor.rgb + vec3(grainValue) * intensity;
    return vec4(grainColor, sceneColor.a);
}

// kernel with dynamic update. Not currently very visible with BlurKernel. 
vec4 applyBlurKernel(in vec2 uv, in vec2 texelSize, float intensity) {

    if (intensity == 0) return texture(screenTexture, uv);

    vec4 blurColor = vec4(0.0);
    float totalWeight = 0.0; // determines how much of the neightboring pixels contribute to final color (has to be normalized (100%) to maintain image brightness
    for(int y = -1; y <= 1; y++) {
        for(int x = -1; x <= 1; x++) {
            vec2 offset = vec2(x, y) * texelSize;
            float weight = blurKernel[(y + 1) * 3 + (x + 1)];
            weight *= intensity; // Apply intensity to the kernel weight
            blurColor += texture(screenTexture, uv + offset) * weight;
            totalWeight += weight;
        }
    }
    if (totalWeight <= 0.0) totalWeight = 1.0;
    return blurColor / totalWeight; // Normalize by the total weight
}

vec4 applyBlur5x5Kernel(in vec2 uv, in vec2 texelSize, float intensity) {

    if (intensity == 0) return texture(screenTexture, uv);

    vec4 blurColor = vec4(0.0);
    float totalWeight = 0.0; // determines how much of the neightboring pixels contribute to final color (has to be normalized (100%) to maintain image brightness
    for(int y = -2; y <= 2 ; y++) {
        for(int x = -2; x <= 2; x++) {
            vec2 offset = vec2(x, y) * texelSize;
            float weight = blur5x5Kernel[(y + 2) * 5 + (x + 2)];
            weight *= intensity; // Apply intensity to the kernel weight
            blurColor += texture(screenTexture, uv + offset) * weight;
            totalWeight += weight;
        }
    }
    if (totalWeight <= 0.0) totalWeight = 1.0;
    return blurColor / totalWeight; // Normalize by the total weight
}



// Define the radialBlur function which takes several parameters:
// color - the original color of the fragment
// uv - the texture coordinates of the current fragment
// texelSize - the size of one texel (i.e., one pixel of the texture) in texture space (usually 1/textureWidth, 1/textureHeight)
// size - the radius of the blur effect
// directions - how many directions around the center the blur will be applied
// quality - how many samples will be taken along each direction to create the blur
// intensity - the strength of the blur effect
vec4 radialBlur(in vec4 color, in vec2 uv, in vec2 texelSize, in float size, in float directions, in float quality, float intensity) {
    // If the intensity of the blur is zero, just return the original color; no processing is needed.
    if (intensity == 0) return color;

    // Define Pi as a constant for a full circle in radians.
    float Pi = 6.28318530718;

    // Start with the original color of the fragment.
    vec4 accumulatedColor = color;
    // Start with a weight of 1 for the original color.
    float weightSum = 1.0;

    // Calculate the radius of the blur based on the size, texelSize, and intensity.
    // The intensity will scale the blur radius.
    vec2 radius = size * texelSize * intensity;

    // Loop over each angle step around the circle to apply the blur in multiple directions.
    for (float angle = 0.0; angle < Pi; angle += Pi / directions) {
        // For each direction, take several samples (determined by quality) along the radius.
        for (float i = 1.0 / quality; i <= 1.0; i += 1.0 / quality) {
            // Calculate the offset for the current sample based on the angle and radius.
            vec2 offset = vec2(cos(angle), sin(angle)) * radius * i;
            // convert each color to luminance before accumulating color and use that as weight. Image brightness is then maintained 
            float lum = luminance(texture(screenTexture, uv + offset).rgb);

            // clamp UV coordinates to texture bounds so that radial blur isn't sampling outside of the screen texture 
            vec2 clampedUV = clamp(uv + offset, 0.0, 1.0);
            // Add the color at this offset to the accumulatedColor.
            accumulatedColor += texture(screenTexture, clampedUV) *  lum;
            // Increment the weightSum for each sample taken.
            weightSum += lum;
        }
    }

    // Clamp the total weights to avoid division by very large numbers.
    weightSum = clamp(weightSum, 1.0, 100.0);

    // Normalize the accumulatedColor by dividing it by the total weight to get the average color.
    accumulatedColor /= weightSum;

    // Return the final color which is the result of the radial blur effect.
    return accumulatedColor;
}
vec4 applyEdgeKernel(in vec2 uv, in vec2 texelSize, float intensity) {

    if (intensity == 0) return texture(screenTexture, uv);

    vec4 edgeColor = vec4(0.0);
    vec4 texColor;
    float kernelResult;
    for(int y = -1; y <= 1; y++) {
        for(int x = -1; x <= 1; x++) {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            texColor = texture(screenTexture, uv + offset);
            float luminance = dot(texColor.rgb, vec3(0.299, 0.587, 0.114)); // converts to grayscale
            kernelResult = edgeKernel[(y + 1) * 3 + (x + 1)];
            edgeColor.r += luminance * kernelResult;
            edgeColor.g += luminance * kernelResult;
            edgeColor.b += luminance * kernelResult;
        }
    }

    edgeColor.rgb = abs(edgeColor.rgb); // Take the absolute value to visualize edges
    edgeColor.rgb = clamp(edgeColor.rgb, 0.0, 1.0); // Clamp values to valid range
    edgeColor *= intensity; // Scale by intensity
    edgeColor.a = 1.0; // Set alpha to 1.0

	return edgeColor;
	}


void main()
{
	
	// get basecolor from texture:
	vec4 basecolor = texture(screenTexture, ftexcoord); // (sampler2D, UV) 
	// basecolor for post processing:
	vec4 postprocess = basecolor;
   
	// 0001 <--mask
	// 0001 <--params

	if (bool(params & INVERT_MASK)) postprocess = invert(postprocess);
	if (bool(params & GRAYSCALE_MASK)) postprocess = grayscale(postprocess);
	if (bool(params & COLORTINT_MASK)) postprocess = colorTint(postprocess, tintRGB);
	if (bool(params & SCANLINE_MASK)) postprocess = scanline(postprocess, ftexcoord, scanlineFrequency, scanlineIntensity);

    // overlay grain effect if GRAIN_MASK is present:
    if (bool(params & GRAIN_MASK)) postprocess = grain(ftexcoord * textureSize(screenTexture, 0), postprocess, time, grainIntensity);
	if (bool(params & BLUR_MASK)) 
		{
			postprocess = applyBlurKernel(ftexcoord, texelSize, blurIntensity);
		}
    if (bool(params & BLUR5x5_MASK)) 
		{
			postprocess = applyBlur5x5Kernel(ftexcoord, texelSize, blurIntensity);
		}

	if (bool(params & EDGE_DETECTION_MASK)) {

        vec2 texelSize = vec2(1.0 / textureWidth, 1.0 / textureHeight);
        postprocess = applyEdgeKernel(ftexcoord, texelSize, edgeIntensity);
       }

    if (bool(params & RADIAL_BLUR_MASK)) 
        {
            vec2 texelSize = vec2(1.0 / textureWidth, 1.0 / textureHeight);
            postprocess = radialBlur(postprocess, ftexcoord, texelSize, uBlurSize, uBlurDirections, uBlurQuality, radialBlurIntensity);
        }

	ocolor = mix(basecolor, postprocess, blend);

}

