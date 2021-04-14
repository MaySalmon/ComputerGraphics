<pre>
this is our DrawLine function:

void Renderer::DrawLine(const glm::ivec2& p1, const glm::ivec2& p2, const glm::vec3& color)
{
	// Bresenham algorithm
	int x1, x2, y1, y2, dx, sx, dy, sy, err1, err2;
	x1 = p1.x; //we set the point (x1,y1) by the first point we get
	y1 = p1.y;
	x2 = p2.x; //we set the point (x2,y2) by the second point we get
	y2 = p2.y;
	dx = abs(x2 - x1); //we want the abs distance between x1 and x2
	dy =abs(y2 - y1); 
	dy = dy * (-1);//we want the -abs distance between y1 and y2 (because y usually in the left side  
	if (x1 < x2) //we want to check the direction of the line 
	{
		sx = 1;
	}
	else
	{
		sx = -1;
	}
	if (y1 < y2) //we want to check the direction of the line 
	{
		sy = 1;
	}
	else
	{
		sy = -1;
	}
	err1 = dx + dy;
	while (true) 
	{
		PutPixel(x1, y1, color);
		if (x1 == x2 && y1 == y2)
			break;
		err2 = 2 * err1;
		if (err2 >= dy)
		{
			x1 += sx;
			err1 += dy;
		}
		if (err2 <= dx)
		{
			y1 += sy;
			err1 += dx;
		}
	}
}
</pre>

<pre>
this is our Render function. 
we drew 4 shapes: circle build from lines, a cube, a flag and a triangle.

void Renderer::Render(const Scene& scene)
{
	glm::ivec2 p1 = glm::ivec2(600, 400); //we set a start point which is the middle of the circle
	glm::ivec2 p2;
	int radius = 100;
	for (int i = 0; i < 350; i++) //the 200 represents the number of lines we want 
	{
		int x = p1.x + radius * cos(i);
		int y = p1.y + radius * sin(i);
		p2= glm::ivec2(x, y);
		DrawLine(p1, p2, glm::vec3(rand()%2, rand()%2, rand()%2)); //we want random colors. we do %2 because the color is bt bits 0/1
		
	}

	//triangle drawing
	DrawLine(glm::vec2(100, 100), glm::vec2(200, 200), glm::vec3(0, 0, 1));
	DrawLine(glm::vec2(200, 200), glm::vec2(300, 100), glm::vec3(0, 0, 1));
	DrawLine(glm::vec2(100, 100), glm::vec2(300,100), glm::vec3(0, 0, 1));

	//cube drawing
	DrawLine(glm::vec2(400, 100), glm::vec2(400, 200), glm::vec3(0, 0, 1));
	DrawLine(glm::vec2(400, 200), glm::vec2(500, 200), glm::vec3(0, 0, 1));
	DrawLine(glm::vec2(500, 200), glm::vec2(500, 100), glm::vec3(0, 0, 1));
	DrawLine(glm::vec2(500, 100), glm::vec2(400, 100), glm::vec3(0, 0, 1));
	DrawLine(glm::vec2(450, 150), glm::vec2(450, 250), glm::vec3(1, 0, 1));
	DrawLine(glm::vec2(450, 250), glm::vec2(550, 250), glm::vec3(1, 0, 1));
	DrawLine(glm::vec2(550, 250), glm::vec2(550, 150), glm::vec3(1, 0, 1));
	DrawLine(glm::vec2(550, 150), glm::vec2(450, 150), glm::vec3(1, 0, 1));
	DrawLine(glm::vec2(400, 100), glm::vec2(450, 150), glm::vec3(1, 0, 0));
	DrawLine(glm::vec2(400, 200), glm::vec2(450, 250), glm::vec3(1, 0, 0));
	DrawLine(glm::vec2(500, 100), glm::vec2(550, 150), glm::vec3(1, 0, 0));
	DrawLine(glm::vec2(500, 200), glm::vec2(550, 250), glm::vec3(1, 0, 0));

	//draw israel flag
	glm::ivec2 f1 = glm::ivec2(905, 150);
	glm::ivec2 f2 = glm::ivec2(604, 150);
	glm::ivec2 f3 = glm::ivec2(905, 40);
	glm::ivec2 f4 = glm::ivec2(604, 40);
	glm::ivec2 ft1 = glm::ivec2(720, 110);
	glm::ivec2 ft2 = glm::ivec2(780, 110);
	glm::ivec2 ft3 = glm::ivec2(750, 80);
	glm::ivec2 ft4 = glm::ivec2(720, 90);
	glm::ivec2 ft5 = glm::ivec2(780, 90);
	glm::ivec2 ft6 = glm::ivec2(750, 120);
	DrawLine(f1, f2, glm::vec3(1, 1, 1));

	for (int i = 40; i <= 150; i++) 
	{
		glm::ivec2 f1 = glm::ivec2(905, i);
		glm::ivec2 f2 = glm::ivec2(604, i);
		DrawLine(f1, f2, glm::vec3(1, 1, 1)); 
	}
	for (int i = 40; i <= 60; i++)  
	{
		glm::ivec2 f1 = glm::ivec2(905, i);
		glm::ivec2 f2 = glm::ivec2(604, i);
		DrawLine(f1, f2, glm::vec3(0, 0, 1)); 
	}
	for (int i = 130; i <= 150; i++)  
	{
		glm::ivec2 f1 = glm::ivec2(905, i);
		glm::ivec2 f2 = glm::ivec2(604, i);
		DrawLine(f1, f2, glm::vec3(0, 0, 1)); 
	}
	DrawLine(f3, f4, glm::vec3(1, 1, 1));
	DrawLine(f1, f3, glm::vec3(1, 1, 1));
	DrawLine(f2, f4, glm::vec3(1, 1, 1));
	DrawLine(ft1, ft2, glm::vec3(0, 0, 1));
	DrawLine(ft1, ft3, glm::vec3(0, 0, 1));
	DrawLine(ft2, ft3, glm::vec3(0, 0, 1));
	DrawLine(ft4, ft5, glm::vec3(0, 0, 1));
	DrawLine(ft4, ft6, glm::vec3(0, 0, 1));
	DrawLine(ft5, ft6, glm::vec3(0, 0, 1));

}
</pre>

the pictures of our shapes: 

![](https://github.com/HaifaGraphicsCourses/computergraphics2021-may-and-saar/blob/master/Images/circle.png)

![](https://github.com/HaifaGraphicsCourses/computergraphics2021-may-and-saar/blob/master/Images/cube.png)

![](https://github.com/HaifaGraphicsCourses/computergraphics2021-may-and-saar/blob/master/Images/triangle.png)

![](https://github.com/HaifaGraphicsCourses/computergraphics2021-may-and-saar/blob/master/Images/flag.png)


