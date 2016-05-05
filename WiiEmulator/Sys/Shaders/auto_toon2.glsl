uniform sampler2D samp9;

out vec4 ocol0;
in vec2 uv0;

uniform vec4 resolution;

void main()
{
 //Changethis to increase the number of colors.
  int numColors =8;

  float4 to_gray = float4(0.3,0.59,0.11,0);
  float x1 = dot(to_gray, texture(samp9, uv0+vec2(1,1)*resolution.zw));
  float x0 = dot(to_gray, texture(samp9, uv0+vec2(-1,-1)*resolution.zw));
  float x3 = dot(to_gray, texture(samp9, uv0+vec2(1,-1)*resolution.zw));
  float x2 = dot(to_gray, texture(samp9, uv0+vec2(-1,1)*resolution.zw));
  float edge = (x1 - x0) * (x1 - x0) + (x3 - x2) * (x3 - x2);
  float4 color = texture(samp9, uv0).rgba;

  float4 c0 = color - float4(edge, edge, edge, edge) * 12;

 

  float red   = 0.0;
  float green = 0.0;
  float blue  = 0.0;
  bool rr = false;
  bool bb = false;
  bool gg = false;
  int count = 1;
  
  float colorN = 0.0;
  float colorB = 0.0;

	 
	 for(count = 1; count <= numColors ; count++){
		 colorN = count / numColors;
		 
		 if ( c0.r <= colorN && c0.r >= colorB && rr == false ){
			if (count == 1){
				if(colorN >= 0.1)
					red = 0.01;
				else
					red = colorN;
			}
			else if (count == numColors)
				red = 0.95;
			else
				red = colorN ;
			
			 rr = true;
		}

		 if (c0.b <= colorN && c0.b >= colorB && bb == false){
			if (count == 1){
				if(colorN >= 0.1)
					blue = 0.01;
				else
					blue = colorN;
			}
			else if (count == numColors)
				blue = 0.95;
			else
				blue = colorN ;
			
			 bb = true;
		}

		 if (c0.g <= colorN && c0.g >= colorB && gg == false){
			if (count == 1){
				if(colorN >= 0.1)
					green = 0.01;
				else
					green = colorN;
			}
			else if (count == numColors)
				green = 0.95 ;
			else
			green = colorN ;
			 gg = true;
		}
			
		colorB = count / numColors;
		if(rr == true && bb == true && gg == true)
			break;
	}


  
  ocol0 = float4(red, green, blue, c0.a);
}