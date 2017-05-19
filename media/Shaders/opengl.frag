
uniform sampler2D myTexture;

void main (void)
{
    vec4 col = texture2D(myTexture, vec2(gl_TexCoord[0]));
    col *= gl_Color;
    gl_FragColor = col * 4.0;
	//gl_FragColor = vec4(1,0,0,1);
}
