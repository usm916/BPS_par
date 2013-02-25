uniform sampler2DRect tex;
uniform vec2 windowSize;
uniform vec2 roiSize;
uniform float offsetALL;
uniform int bInvert;

varying vec2 texcoord0;

void main()
{
	//vec2 offset			= ( gl_FragCoord.xy/windowSize - vec2( 0.5 ) ) * 0.005;
    vec2 offset			= ( gl_FragCoord.xy/windowSize - vec2( 0.5 ) ) * offsetALL;
    float rChannel;
    float gChannel;
    float bChannel;
    
//    if( roiSize.x < texcoord0.x && roiSize.y > texcoord0.x){
//        rChannel		= 1.0-texture2DRect( tex, texcoord0 ).r;
//        gChannel		= 1.0-texture2DRect( tex, texcoord0 + offset ).g;
//        bChannel		= 1.0-texture2DRect( tex, texcoord0 + offset * 2.0 ).b;
//    }else{
        rChannel		= texture2DRect( tex, texcoord0 ).r;
        gChannel		= texture2DRect( tex, texcoord0 + offset ).g;
        bChannel		= texture2DRect( tex, texcoord0 + offset * 2.0 ).b;
//    }

//	float rChannel		= texture2DRect( tex, texcoord0 ).r;
//	float gChannel		= texture2DRect( tex, texcoord0 + offset ).g;
//	float bChannel		= texture2DRect( tex, texcoord0 + offset * 2.0 ).b;
	vec3 finalCol		= vec3( rChannel, gChannel, bChannel );
	
	if(bInvert==0){
        gl_FragColor.rgb	= finalCol;
    }else{
        gl_FragColor.rgb	= vec3(1.0) - finalCol;
    }
	gl_FragColor.a		= 1.0;
}