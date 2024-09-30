# Raytracing Project

By Zeynep Tosun, Pascal Hollinger and Philipp Rauser


<div style = "text-align:center">
    <img src="markdown/assets/header.png" alt="a lot of spheres" width="800"/>
    <br />
</div>
<em>header.json</em>

---
## Table of Contents
- [Showcase of the program](#showcase-of-the-program)
- [How to render your own scene](#how-to-render-your-own-scene)
- [installation guide](markdown/installation.md)
- [description of the code](markdown/code_description.md)

---

## Showcase of the program
Our raytracing program is designed to render complex scenes with high efficiency and detail. Here are some of the key features: \
\
**Object Rendering**: The program can render both spheres and planes (halfspaces) with precision. \
**Color Properties**: Objects can be assigned various color properties, including ambient, diffuse, and specular colors. \
**Reflective and Refractive Surfaces**: Objects can be made reflective or refractive. For refractive objects, a refraction index can be specified to control how light bends as it passes through. \
**Multithreading**: To enhance performance, the program utilizes multithreading, significantly speeding up the rendering process. \
**Sky Color Rendering**: If a ray does not intersect any object, the program renders a sky color based on the angle of the ray. \
**Normal Mapping**: Normal maps can be applied to both spheres and planes, allowing for the rendering of intricate surface details. \

**Some Examples**:

<table>
<tr>
    <td>
        <img src="markdown/assets/spheres.png" alt="scene1"/>
        <br />
        <em>spheres.json</em>
    </td>
    <td>
        <img src="markdown/assets/reflection.png" alt="scene2"/>
        <br />
        <em>reflection.json</em>
    </td>
    <td>
        <img src="markdown/assets/refracted.png" alt="scene3"/>
        <br />
        <em>refracted.json</em>
    </td>
</tr>
<tr>
    <td>
        <img src="markdown/assets/reflection_vs_refraction.png" alt="scene4"/>
        <br />
        <em>reflection_vs_refraction.json</em>
    </td>
    <td>
        <img src="markdown/assets/textures.png" alt="scene5"/>
        <br />
        <em>textures.json</em>
    </td>
</tr>
</table>

---

## How to render your own scene 
Ready to create your own stunning visual masterpiece? Make sure you’ve correctly built the program by following the [installation guide](markdown/installation.md).

Scene Basics
Each scene starts with a screen. You’ll need to specify the observer's position, the screen's location, its dimensions, and the pixel density.

Then, we define the medium (aka, the atmosphere of your scene). This involves setting the ambient light, the medium's refractive index, and the recursion depth (how deep the rabbit hole goes when calculating reflections).

Defining Light Sources
Light sources bring your scene to life! Each source needs a position and intensity. Want colored lights? No problem—just define the intensity for each color channel and you're set.

Adding Objects
Objects are where things get interesting. Each object requires a position, a refraction index, and a color (broken down into its component values). If you’re working with spheres, you’ll also need to set the radius, while Halfspace require a normal vector.

Transforming Objects
Feel free to combine these transformations (rotation, transformation and scaling) by nesting them around the object. And yes, they stack.

Merging Objects
To combine objects into a single entity, use the union operator. This allows you to create complex shapes by merging multiple objects together. (union, intersection, exclusion) 
```json
{
    "screen": {
        "dpi": 200,
        "dimensions": [2, 4],
        "position": [-1, -2, -10],
        "observer": [0, 0, -15]
    },

    "medium": {
        "ambient": [0.1, 0.1, 0.1],
        "index": 1,
        "recursion": 1
    },

    "sources": [
        {
            "position": [50, 50, -10],
            "intensity": [0.82, 0.97, 0.98]
        }
    ],

    "objects": [
        {
            "scaling":{
                "factors": [1.0,1.0,1.0],
                "subject": {
                    "translation":{
                        "factors":[0.0, 0.0, 0.0],
                        "subject":{
                            "halfSpace": {
                                "position": [-2, 0, 0],
                                "normal": [1,0,0],
                                "color": {
                                    "ambient": [1, 1, 1],
                                    "diffuse": [1, 1, 1],
                                    "specular": [1, 1, 1],
                                    "reflected": [0.1,0.1,0.1],
                                    "refracted": [0,0,0],
                                    "shininess": 1
                                },
                                "index": 1.5
                            }
                        }
                    }
                }}
        },
        {"union": [
            {"scaling": {
                "factors": [4, 4, 4],
                "subject": {
                    "sphere": {
                        "position": [-1, 0, 0],
                        "radius": 1.0,
                        "color": {
                            "ambient": [1, 1, 1],
                            "diffuse": [1, 1, 1],
                            "specular": [1, 1, 1],
                            "reflected": [1, 1, 1],
                            "refracted": [1, 1, 1],
                            "shininess": 1
                        },
                        "index": 1
                    }
                }
            }

            },
            {"translation": {
                "factors": [0, 0, 2.5],
                "subject": {
                    "halfSpace": {
                        "position": [-2, 0, 0],
                        "normal": [-2, 0, 0],
                        "color": {
                            "ambient": [1, 1, 1],
                            "diffuse": [1, 1, 1],
                            "specular": [1, 1, 1],
                            "reflected": [1, 1, 1],
                            "refracted": [1, 1, 1],
                            "shininess": 1
                        },
                        "index": 1
                    }
                }
            }

            }
        ]
        }
    ]
}
```
