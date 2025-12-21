#type vertex
#version 450 core
layout(location = 0) in vec2 a_Position;
layout(location = 1) in vec2 a_TexCoord;

layout(location = 0) out vec2 v_TexCoord;

void main()
{
    gl_Position = vec4(a_Position.x, a_Position.y, 0.0, 1.0);
    v_TexCoord = a_TexCoord;
}

#type fragment
#version 450 core

layout(location = 0) out vec4 FragColor;
layout(binding = 0) uniform UBO {
    sampler2D screenTexture;
} ubo;

layout(location = 0) in vec2 v_TexCoord;

// 反相

// void main()
// {
//     FragColor = vec4(vec3(1.0 - texture(screenTexture, v_TexCoord)), 1.0);
// }

// 核效果

// const float offset = 1.0 / 300.0;

// void main()
// {
//     vec2 offsets[9] = vec2[](
//         vec2(-offset, offset),  // 左上
//         vec2(0.0f, offset),     // 正上
//         vec2(offset, offset),   // 右上
//         vec2(-offset, 0.0f),    // 左
//         vec2(0.0f, 0.0f),       // 中
//         vec2(offset, 0.0f),     // 右
//         vec2(-offset, -offset), // 左下
//         vec2(0.0f, -offset),    // 正下
//         vec2(offset, -offset)   // 右下
//     );

//     float kernel[9] = float[](
//         1.0 / 16, 2.0 / 16, 1.0 / 16,
//         2.0 / 16, 4.0 / 16, 2.0 / 16,
//         1.0 / 16, 2.0 / 16, 1.0 / 16);

//     vec3 sampleTex[9];
//     for (int i = 0; i < 9; i++)
//     {
//         sampleTex[i] = vec3(texture(screenTexture, v_TexCoord.st + offsets[i]));
//     }
//     vec3 col = vec3(0.0);
//     for (int i = 0; i < 9; i++)
//         col += sampleTex[i] * kernel[i];

//     FragColor = vec4(col, 1.0);
// }

// 灰度
// void main()
// {
//     FragColor = texture(screenTexture, v_TexCoord);
//     float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
//     FragColor = vec4(average, average, average, 1.0);
// }

void main()
{
    FragColor = texture(ubo.screenTexture, v_TexCoord);
}