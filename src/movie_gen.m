% author: Nate Sutton 2021
%
% references: 
% https://www.mathworks.com/matlabcentral/answers/154659-how-to-create-animation-of-matlab-plotting-points-on-a-graph
% https://www.mathworks.com/matlabcentral/answers/220094-how-do-i-make-a-video-of-a-graph-with-axes-and-axes-labels
% https://www.mathworks.com/matlabcentral/answers/285058-how-to-keep-the-numbers-on-the-colorbar-scale-of-a-surface-plot-fixed

clear all;
clc;
time=999; % time steps, use (end frame - 1) = time
t=[0:0.1:(time*.1)];
hFigure = figure;
numberOfFrames = length(t);
% Set up the movie structure.
% Preallocate recalledMovie, which will be an array of structures.
% First get a cell array with all the frames.
allTheFrames = cell(numberOfFrames,1);
vidHeight = 337;%342;
vidWidth = 442;%434;
allTheFrames(:) = {zeros(vidHeight, vidWidth, 3, 'uint8')};
% Next get a cell array with all the colormaps.
allTheColorMaps = cell(numberOfFrames,1);
allTheColorMaps(:) = {zeros(256, 3)};
% Now combine these to make the array of structures.
myMovie = struct('cdata', allTheFrames, 'colormap', allTheColorMaps);
set(gcf, 'nextplot', 'replacechildren'); 
% Need to change from the default renderer to zbuffer to get it to work right.
% openGL doesn't work and Painters is way too slow.
set(gcf, 'renderer', 'zbuffer');
caxis manual;          % allow subsequent plots to use the same color limits

%[A,map,alpha] = imread('/home/nmsutton/Dropbox/CompNeuro/gmu/research/sim_project/code/move_test/media/grid.png');
%A = imread('/home/nmsutton/Dropbox/CompNeuro/gmu/research/sim_project/code/move_test/media/grid.png');

for frameIndex = 1 : numberOfFrames
  filename = strcat('../output/firing_t',int2str(frameIndex),'.csv');
  [imgfile,delimiterOut]=importdata(filename);
  cla reset;
  % Enlarge figure to full screen.
  %   set(gcf, 'Units', 'Normalized', 'Outerposition', [0, 0, 1, 1]);
  imagesc(imgfile);
  %imresize(img,[343 434]);
  axis('tight')
  xlabel('neuron position on x axis') 
  ylabel('neuron position on y axis')
  shading interp;
  %caxis([0 3.5])
  caxis([0 5.5])
  cb = colorbar;
  %set(cb, 'ylim', [0 4]); % set colorbar range
  set(cb, 'ylim', [0 5.5]); % set colorbar range
  %zlim([0, 1]);
  caption = sprintf('Grid cell firing amounts, t = %.0f ms', frameIndex);
  title(caption, 'FontSize', 15);
  %hold on;
  %hb = imshow(A, [], 'Colormap', gray(256));
  %hb = imshow(A);
  %hb.AlphaData = alpha;
  thisFrame = getframe(gcf);
  myMovie(frameIndex) = thisFrame;
end
close(hFigure);
myMovie(1) = []; % remove first frame causing issues due to wrong size
v = VideoWriter('firing.avi'); % Create a VideoWriter object to write the video out to a new, different file.
open(v)
writeVideo(v,myMovie) % Write the movie object to a new video file.
close(v)