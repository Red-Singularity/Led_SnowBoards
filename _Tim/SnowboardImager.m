clc
clear all
close all

%BlockColorImage('font.png',13,21);
ImageDraw(5,8,80);
%|||||||||||||main functions||||||||||||||||

%each snowboard pixel is set to the average color of the surrounding
%pixels, ignoring black if black occupies < 50% of the surrounding pixels
%and defaulting to black if black occipies > 50%
%USE THIS FOR ORGANIC IMAGES
function AveragedColorImage(I,xPixels,yPixels)
image = imread(I);  %reads image file, if script isn't in the same location as the image file then must include the full file path for image file

%add black borders to image so you don't exceed array bounds when selecting
imagex = length(image(1,:,1));
imagey = length(image(:,1,1));
image = [zeros(500,imagex,3);image;zeros(500,imagex,3)];                %adds 500 black pixels to top and bottom of image
image = [zeros(imagey+1000,500,3),image,zeros(imagey+1000,500,3)];      %adds 500 black pixels to sides of image

%open a new figure showing the image with the functions mouseMove,
%readMousePos, and keyPress. Don't move on till the figure is closed.
[mousePos,scale] = GUIgrid(image,xPixels,yPixels);

%use the mouse position to read only the selected box from the image
x = round(mousePos(1,1));
y = round(mousePos(1,2));
image = image(y:y+yPixels*scale,x:x+xPixels*scale,:);

%remove white space from the image
image = uint8(removeWhiteSpace(image,240));

%create an empty array which will be the down-scaled image
deresImage = zeros(yPixels,xPixels,3);

%for each pixel in deresImage set to the average color of the respective
%pixel group in Image (pixel group is a scaleXscale square)
%deresImage will be a yPixels by xPixels by 3 array of 0-1 values
for x = 1:xPixels
    for y = 1:yPixels
        %first calculate the average color of the pixel group in image
        averageColor = single(zeros(1,1,3));
        darkPixels = 0;     %tracks how many dark pixels are present (which aren't counted by average color)
        for i = 1:scale
            for j = 1:scale
                imy = (y-1)*scale+j;
                imx = (x-1)*scale+i;
                if image(imy,imx,:) ~= zeros(1,1,3);                        %don't count the black pixels
                    averageColor = single(image(imy,imx,:))+averageColor;   %sum the color values
                else
                    darkPixels = darkPixels + 1;
                end
            end
        end
        if darkPixels < 0.5*scale^2                                         %if < 50% of pixels were black
            averageColor = averageColor./((scale^2-darkPixels)*255);        %divide sum by scale^2-darkPixels to average and by 255 to convert uint8 to 0-1
        else
            averageColor = single(zeros(1,1,3));
        end
        deresImage(y,x,:) = averageColor;
    end
end

imshow(deresImage);

outputArray(deresImage,xPixels,yPixels);
end

%each snowboard pixel is set to the most prevalent color in the surrounding
%pixels
%USE THIS FOR 8-BIT/PIXELATED IMAGES
function BlockColorImage(I,xPixels,yPixels)
image = imread(I);  %reads image file, if script isn't in the same location as the image file then must include the full file path for image file

%add black borders to image so you don't exceed array bounds when selecting
imagex = length(image(1,:,1));
imagey = length(image(:,1,1));
image = [zeros(500,imagex,3);image;zeros(500,imagex,3)];                %adds 500 black pixels to top and bottom of image
image = [zeros(imagey+1000,500,3),image,zeros(imagey+1000,500,3)];      %adds 500 black pixels to sides of image

[mousePos,scale] = GUIgrid(image,xPixels,yPixels);

%use the mouse position to read only the selected box from the image
x = round(mousePos(1,1));
y = round(mousePos(1,2));
image = image(y:y+yPixels*scale,x:x+xPixels*scale,:);

%remove white space from the image
%image = round(removeWhiteSpace(image,240));

%create an empty array which will be the down-scaled image
deresImage = zeros(yPixels,xPixels,3);

%for each pixel in deresImage set to the most prevalent color of the pixel
%group in Image (pixel grou[ is a scaleXscale square)
%deresImage will be a yPixels by xPixels by 3 array of 0-1 values
for x = 1:xPixels
    for y = 1:yPixels
        %first calculate the most prevalent color in the pixel group
        colorList = [0,0,0,0;0,0,0,0];      %this will track the colors used
        pixelColor = zeros(1,1,3);
        for i = 1:scale
            for j = 1:scale
                imy = (y-1)*scale+j;
                imx = (x-1)*scale+i;
                %get RGB values from image, and add one cuz matlab is dumb
                %and indexes starting at 1
                R = image(imy,imx,1);
                G = image(imy,imx,2);
                B = image(imy,imx,3);
                
                colorfound = false;
                %search through the colorList for if this value exists
                for n = 1:length(colorList(:,1))
                    %if the color is in the list then increment and break
                    if [R,G,B] == colorList(n,1:3)
                        colorList(n,4) = colorList(n,4) + 1;
                        colorfound = true;
                        break
                    end
                end
                
                %if the color wasn't found in the colorList, add it
                if ~colorfound
                    colorList = [colorList;[R,G,B,1]];
                end
            end
        end
        %select the highest used color in colorList
        max = 0;
        for n = 1:length(colorList(:,1))
            if colorList(n,4) > max
                pixelColor(:,:,1) = colorList(n,1);
                pixelColor(:,:,2) = colorList(n,2);
                pixelColor(:,:,3) = colorList(n,3);
                max = colorList(n,4);
            end
        end
        %set deres image color
        pixelColor = single(pixelColor)/255;
        deresImage(y,x,:) = pixelColor;
    end
end

imshow(deresImage);

outputArray(deresImage,xPixels,yPixels);
end

%draw white pixels on a black backround by hand
%USE THIS FOR CREATING CUSTOM IMAGES
function ImageDraw(xPixels,yPixels,scale)
    image = zeros(yPixels*scale,xPixels*scale,3);
    stopCon = false;
    mousePoint = [0,0];
    
    %repeat until the user presses a key while in the figure
    while ~stopCon
        %user selects a pixel. If it's white it becomes black, if it's
        %black it becomes white. If the user presses a key the loop breaks
        [mousePoint,stopCon] = GUIdraw(image,xPixels,yPixels,scale);
        x = floor(mousePoint(1,1)/scale);
        y = floor(mousePoint(1,2)/scale);
        if(~stopCon)
            if image(round((y+0.5)*scale)+1,round((x+0.5)*scale)+1,1) ==  0
                image(y*scale+1:(y+1)*scale+1,x*scale+1:(x+1)*scale+1,1) = 1;
                image(y*scale+1:(y+1)*scale+1,x*scale+1:(x+1)*scale+1,2) = 1;
                image(y*scale+1:(y+1)*scale+1,x*scale+1:(x+1)*scale+1,3) = 1;
            else
                image(y*scale+1:(y+1)*scale+1,x*scale+1:(x+1)*scale+1,1) = 0;
                image(y*scale+1:(y+1)*scale+1,x*scale+1:(x+1)*scale+1,2) = 0;
                image(y*scale+1:(y+1)*scale+1,x*scale+1:(x+1)*scale+1,3) = 0;
            end
        end
    end
    
    %output the final array
    output = '{';
    for y = 0:yPixels - 1
        output = strcat(output,'{');
        for x = 0:xPixels - 1
            output = strcat(output,num2str(image(round((y+0.5)*scale)+1,round((x+0.5)*scale)+1,1)));
            output = strcat(output,',');
        end
        output = output(1:end-1);       %delete the last character of output b/c there is a hanging comma
        output = strcat(output,'},');
    end
    output = output(1:end-1);       %delete the last character of output b/c there is a hanging comma
    output = strcat(output,'};');
    disp(output);
end

%|||||||||||supporting functions|||||||||||||

%takes an image, plots it, and allows the user to select the grid to
%convert to the snowboard-level image
function [mousePoint,scale] = GUIgrid(image,xPixels,yPixels)
scale = 40; %default scale value

%open a new figure showing the image with the functions mouseMove,
%readMousePos, and keyPress. Don't move on till the figure is closed.
fig = figure('NumberTitle','off','Name','Press Any Key to Re-Size Grid, Left Click to Select',...
    'WindowButtonMotionFcn', {@mouseMove,xPixels,yPixels},...
    'WindowButtonDownFcn',@readMousePos,'KeyPressFcn',@keyPress);
imshow(image);
uiwait(fig);
return

%detects a key press and allows user to change scale
function keyPress (object, eventdata)
    answer = inputdlg(strcat('Prev Scale Value:',32,num2str(scale),32,'New Scale Value:',32));
    scale = str2num(answer{1});
end

%detects mouse movement and displays the scaled rectangle on image
function mouseMove (object, eventdata,x,y)
    C = get(gca, 'CurrentPoint');
    prevGrid = findobj('tag','gridLine');
    delete(prevGrid);
    xi = round(C(1,1));
    yi = round(C(1,2));
    for row = yi:scale:yi+(y*scale)
        line([xi,xi+(x*scale)],[row,row],'Color','r','tag','gridLine');
    end
    for col = xi:scale:xi+(x*scale)
        line([col,col],[yi,yi+(y*scale)],'Color','r','tag','gridLine');
    end
end

%detects a left click, reads mouse position then closes figure
function readMousePos (object, eventdata)
    mousePoint = get(gca, 'CurrentPoint');
    close all
end
end

%takes a black image, shows it with a grid for pixel borders and allows the user
%to select pixels on the grid to change to white
function [mousePoint,stopCon] = GUIdraw(image,xPixels,yPixels,scale)
stopCon = false;
%open a new figure showing the image with the functions mouseMove,
%readMousePos, and keyPress. Don't move on till the figure is closed.
fig = figure('NumberTitle','off','Name','Left Click to Select Point',...
    'WindowButtonDownFcn',@readMousePos,'KeyPressFcn',@keyPress);
imshow(image);

%plot grids on the figure to denote where the pixel borders are
for row = 0:scale:yPixels*scale
    line([0,xPixels*scale],[row,row],'Color','r');
end
for col = 0:scale:yPixels*scale
    line([col,col],[0,yPixels*scale],'Color','r');
end
uiwait(fig);
return
    
%stops the script and closes all figures
function keyPress (object, eventdata)
    mousePoint = [0,0];
    stopCon = true;
    close all
    return
end

%detects a left click, reads mouse position then closes figure
function readMousePos (object, eventdata)
    mousePoint = get(gca, 'CurrentPoint');
    close all
end
end

%outputs the deresImage array in a format ready to be copy and pasted into
%arduino ID
function outputArray(deresImage,xPixels,yPixels)
output = '{';
for y = 1:yPixels
    output = strcat(output,'{');
    for x = 1:xPixels
        output = strcat(output,'{');
        for c = 1:3
            output = strcat(output,num2str(deresImage(y,x,c)),',');    %put in each bit of color data
        end
        output = output(1:end-1);       %delete the last character of output b/c there is a hanging comma
        output = strcat(output,'},');
    end
    output = output(1:end-1);       %delete the last character of output b/c there is a hanging comma
    output = strcat(output,'},');
end
output = output(1:end-1);       %delete the last character of output b/c there is a hanging comma
output = strcat(output,'};');
disp(output);
end

%sets a prospective white pixel (average pixel brightness of whitePoint to black (0,0,0)
function resultImage = removeWhiteSpace(inputImage,whitePoint) 
    resultImage = single(inputImage);
        
    for x = 1:length(resultImage(1,:,1))
        for y = 1:length(resultImage(:,1,1))
            colorMagnitude = (resultImage(y,x,1)+...                        %average color channel brightness
                resultImage(y,x,2)+resultImage(y,x,3))/3;
                
            colorDeviation = sqrt((resultImage(y,x,1)-colorMagnitude)^2+... %color channel deviation
                (resultImage(y,x,2)-colorMagnitude)^2+...
                (resultImage(y,x,3)-colorMagnitude)^2);
                
            if colorMagnitude >= whitePoint && colorDeviation < 0.08
                resultImage(y,x,:) = zeros(1,1,3);
            end
        end
    end
    return
end

