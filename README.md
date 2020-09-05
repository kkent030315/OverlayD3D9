# OverlayD3D9
Simple overlay using `D3D9`

```cpp
constexpr auto color = D3DCOLOR_RGBA(255, 0, 0, 255);

renderer::DrawString(XORSTR("I AM A GOOD BOY"), { 15, 45 }, color, false, true, detail::d3d9_font);

renderer::DrawRectangle({ 1920 / 2, 1080 / 2, 60, 120 }, D3DCOLOR_ARGB(255, 155, 000, 155), true);
renderer::DrawRectangle({ 1920 / 3, 1080 / 3, 60, 120 }, D3DCOLOR_ARGB(255, 155, 055, 155), true);
renderer::DrawRectangle({ 1920 / 4, 1080 / 4, 60, 120 }, D3DCOLOR_ARGB(200, 055, 155, 155), true);
renderer::DrawRectangle({ 1920 / 5, 1080 / 5, 60, 120 }, D3DCOLOR_ARGB(155, 055, 255, 155), true);
```

# Requirements
Only supports win10 x64

# Features
- FPS Counter  
- String Xoring  
- Draw Strings  
L Custom Colors  
L Auto Centering  
L Outline For Visibility  
- Draw Lines  
L Custom Colors  
- Draw Rectangles  
L Custom Colors  
L Outline For Visibility  
- Draw Circles  
L Custom Colors
