IRender_Light  *CLASS_IMPL::GetLight(int target)
{
	if (target == 0)
		return target_0._get();
	else
	if (target_1)
		return target_1._get();

	return NULL;
}


void CLASS_IMPL::SetAnimation (LPCSTR name)
{
	lanim =	LALib.FindItem(name);
}

void CLASS_IMPL::SetBrightness (float brightness)
{
	Fcolor c;
	c = target_0->get_color();
	if (fBrightness > 0)
		c.mul_rgb ( 255.f / fBrightness );

	fBrightness = brightness;
	SetColor (c); // immediate update
}

void CLASS_IMPL::SetColor (const Fcolor &color, int target)
{
	Fcolor c = color;	
	c.mul_rgb ( fBrightness / 255.f );
	switch (target)
	{
		case 0: target_0->set_color(c); break;
		case 1: if(target_1) target_1->set_color(c); break;
		case 2: if(target_2) target_2->set_color(c); break;			
	}
	
}

void CLASS_IMPL::SetRGB (float r, float g, float b, int target)
{
	Fcolor c;
	c.a = 1;
	c.r = r;
	c.g = g;
	c.b = b;
	SetColor(c, target);
}

void CLASS_IMPL::SetAngle (float angle, int target)
{	
	switch (target)
	{
	case 0: target_0->set_cone(angle); break;
	case 1: if(target_1) target_1->set_cone(angle); break;	
	}	
}

void CLASS_IMPL::SetRange (float range, int target)
{	
	switch (target)
	{
		case 0: target_0->set_range(range); break;
		case 1: if(target_1) target_1->set_range(range); break;
		case 2: if(target_2) target_2->set_radius(range); break;
	}
}

void CLASS_IMPL::SetTexture (LPCSTR texture, int target)
{
	switch (target)
	{
	case 0: target_0->set_texture(texture); break;
	case 1: if(target_1) target_1->set_texture(texture); break;
	case 2: if(target_2) target_2->set_texture(texture); break;
	}
}
void CLASS_IMPL::SetVirtualSize (float size, int target)
{
	switch (target)
	{
	case 0: target_0->set_virtual_size(size);
	case 1: if(target_1) target_1->set_virtual_size(size);
	}
}
