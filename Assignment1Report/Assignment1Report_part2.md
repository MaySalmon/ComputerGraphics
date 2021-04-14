<pre>

We have in main.cpp a GUI code that states the scale translation of the model. we initialize it so it will be inside the window. 

the code : 

	ImGui::End();

	static float f = 0.0f;
	static int size = 120;
	static glm::vec3 translate = { 0.0f, 0.0f, 0.0f };
	static glm::vec3 rotation = { 0.0f, 0.0f, 0.0f };
	static glm::vec3 scale = { 1.0f, 1.0f, 1.0f };
	static int scale_uniformly = 1;
	static float normal_size = 1.0f;

if we will change to: "static glm::vec3 scale = { 2000.0f, 2000.0f, 2000.0f };" instead, the model would go out from window.

those variables are used in the Rendere.cpp to draw the models inside the window correctly.

</pre>


<pre>

We uploaded the bunny model and took 3 different screenshot with different transfotamtions.

</pre>

<img src="https://github.com/HaifaGraphicsCourses/computergraphics2021-may-and-saar/blob/master/Images/buunyscreenshot.png">
<img src="https://github.com/HaifaGraphicsCourses/computergraphics2021-may-and-saar/blob/master/Images/bunnyscreenshot2.png">
<img src="https://github.com/HaifaGraphicsCourses/computergraphics2021-may-and-saar/blob/master/Images/bunnyscreenshot3.png">

<pre>

We changed to camera model and took 2 different screenshot with different transfotamtion.
The data of the transfotamtion of previous model (the bunny) is still exist. if we press again the bunny model button, we will see the model as we recentky changed it. 

</pre>

<img src="https://github.com/HaifaGraphicsCourses/computergraphics2021-may-and-saar/blob/master/Images/camerascreenshot.png">
<img src="https://github.com/HaifaGraphicsCourses/computergraphics2021-may-and-saar/blob/master/Images/camerascreenshot2.png">

<pre>

We can also upload all the models at the same time and change each transfotamtion independently.

</pre>

<img src="https://github.com/HaifaGraphicsCourses/computergraphics2021-may-and-saar/blob/master/Images/allmodelsscreenshot.png">