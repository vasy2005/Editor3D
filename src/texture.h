#pragma once

void OpenTexture(char*& bitmap, int& bmWidth, int& bmHeight)
{
	OPENFILENAME dialogue = { 0 };
	char filePath[512] = "";

	dialogue.lStructSize = sizeof(dialogue);
	dialogue.hwndOwner   = flags->windowHandle;
	dialogue.lpstrFilter = "Bitmap Image (*.bmp)\0*.bmp\0";
	dialogue.lpstrFile   = filePath;
	dialogue.nMaxFile    = 512;
	dialogue.Flags       = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	dialogue.lpstrDefExt = "bmp";

	int error = GetOpenFileName(&dialogue); // open the dialogue window

	if (error != 0) // success
	{
		FILE* input = fopen(filePath, "rb");

		// get width and height at offset 18
		fseek(input, 18, SEEK_SET);
		fread(&bmWidth,  4, 1, input);
		fread(&bmHeight, 4, 1, input);

		char format;
		fseek(input, 28, SEEK_SET);
		fread(&format, 1, 1, input);

		if (bitmap != NULL)
			delete[] bitmap;
		bitmap = new char[24 + 4 * bmWidth * bmHeight];
		getimage(0, 0, bmWidth - 1, bmHeight - 1, bitmap); // take a screenshot. it creates the file header automatically (first 24 bytes)

		fseek(input, 54, SEEK_SET); // skip actual file header which is different from the one graphics.h uses because why not

		// for some reason bitmaps are stored upside down so i cant just fread the whole thing i gotta do this 
		if (format == 32)
		{
			for (int y = bmHeight - 1; y >= 0; y--)
				for (int x = 0; x < bmWidth; x++)
					fread(bitmap + 24 + 4 * (y * bmWidth + x), 4, 1, input);
		}
		else if (format == 24)
		{
			for (int y = bmHeight - 1; y >= 0; y--)
				for (int x = 0; x < bmWidth; x++)
					fread(bitmap + 24 + 4 * (y * bmWidth + x), 3, 1, input);
			                                                // ^~~~~~ it changed!
		}
		else
		{
			cout << "ERROR: Format not supported!\n";
		}

		fclose(input);
	}

	flags->updateWindow = 1;
}



void ClearFrame()
{
	for(int y = 0; y < WINDOW_HEIGHT; y++)
		for (int x = 0; x < WINDOW_WIDTH; x++)
		{
			canvas[24 + 4 * (y * WINDOW_WIDTH + x) + 0] = 0;
			canvas[24 + 4 * (y * WINDOW_WIDTH + x) + 1] = 0;
			canvas[24 + 4 * (y * WINDOW_WIDTH + x) + 2] = 0;
			canvas[24 + 4 * (y * WINDOW_WIDTH + x) + 3] = 0;
		}
}

void Screenshot()
{
	getimage(0, 0, WINDOW_WIDTH - 1, WINDOW_HEIGHT - 1, canvas);
}

void DrawFrame()
{
	putimage(0, 0, canvas, COPY_PUT);
}


void FastSetPixel(int x, int y, int color)
{
	if (x < 0 || x >= WINDOW_WIDTH || y < 0 || y >= WINDOW_HEIGHT)
		return;

	canvas[24 + 4 * (y * WINDOW_WIDTH + x) + 0] = GetBValue(color); // blue
	canvas[24 + 4 * (y * WINDOW_WIDTH + x) + 1] = GetGValue(color); // green
	canvas[24 + 4 * (y * WINDOW_WIDTH + x) + 2] = GetRValue(color); // red
	canvas[24 + 4 * (y * WINDOW_WIDTH + x) + 3] = 0;                // alpha
}

int FastGetPixel(int x, int y, char* bitmap, int width, int height)
{
	if (x < 0 || x >= width || y < 0 || y >= height)
		return RGB(0, 0, 0);

	int b = bitmap[24 + 4 * (y * width + x) + 0];
	int g = bitmap[24 + 4 * (y * width + x) + 1];
	int r = bitmap[24 + 4 * (y * width + x) + 2];

	return RGB(r, g, b);
}

float Baryocentric(Vector3 first, Vector3 second, Vector3 point)
{
	return (second.x - first.x) * (point.y - first.y) - (second.y - first.y) * (point.x - first.x);
}

void DrawTexturedTriangle(Vector3 third, Vector3 second, Vector3 first, Vector2 uv3, Vector2 uv2, Vector2 uv1, char* texture, int textureWidth, int textureHeight)
{
	float minX = min(first.x, min(second.x, third.x));
	float minY = min(first.y, min(second.y, third.y));
	float maxX = max(first.x, max(second.x, third.x));
	float maxY = max(first.y, max(second.y, third.y));

	for (int y = minY; y <= maxY; y++)
		for (int x = minX; x <= maxX; x++)
		{
			Vector3 point = { x, y };

			float det1 = Baryocentric(second,  first, point); if (det1 < 0) continue;
			float det2 = Baryocentric( third, second, point); if (det2 < 0) continue;
			float det3 = Baryocentric( first,  third, point); if (det3 < 0) continue;

			float det4 = Baryocentric(second, first, third);

			float l1 = det1 / det4;
			float l2 = det2 / det4;
			float l3 = det3 / det4;

			float posX = uv1.x * l1 + uv2.x * l2 + uv3.x * l3;
			float posY = uv1.y * l1 + uv2.y * l2 + uv3.y * l3;

			posX = posX * (textureWidth  - 1);
			posY = textureHeight - 1 - posY * (textureHeight - 1); // scaderea e ca sa intorc textura cu susu in jos ca sa fie desenata corect

			int color = FastGetPixel(posX, posY, texture, textureWidth, textureHeight);

			FastSetPixel(x, y, color);
		}
}