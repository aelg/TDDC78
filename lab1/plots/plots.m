% ./matlab_format.py slurm-604666.out 32_cores
figure(1)
im1_32_cores = [0.0186079 0.0156629 0.028414 0.045949];
im1_32_cores_radius = [1 10 50 100];
im2_32_cores = [0.011487 0.014153 0.046895 0.077534];
im2_32_cores_radius = [1 10 50 100];
im3_32_cores = [0.015825 0.030442 0.0700002 0.108959];
im3_32_cores_radius = [1 10 50 100];
im4_32_cores = [0.039016 0.0772309 0.243409 0.47332];
im4_32_cores_radius = [1 10 50 100];
plot(im1_32_cores_radius, im1_32_cores, '-x', im2_32_cores_radius, im2_32_cores, '-x', im3_32_cores_radius, im3_32_cores, '-x', im4_32_cores_radius, im4_32_cores, '-x');
legend('im1', 'im2', 'im3', 'im4', 'Location', 'NorthWest');
xlabel('Radius');
ylabel('Run time');
title('Averaging filter run on 32 cores');

%./matlab_format.py slurm-604660.out 16_cores
im1_16_cores = [0.0593829 0.0123641 0.0341439 0.0552392];
im1_16_cores_radius = [1 10 50 100];
im2_16_cores = [0.00992084 0.018604 0.060745 0.120128];
im2_16_cores_radius = [1 10 50 100];
im3_16_cores = [0.014132 0.0327652 0.09953 0.189664];
im3_16_cores_radius = [1 10 50 100];
im4_16_cores = [0.0528212 0.125994 0.448156 0.885437];
im4_16_cores_radius = [1 10 50 100];
figure(2)
plot(im1_16_cores_radius, im1_16_cores, '-x', im2_16_cores_radius, im2_16_cores, '-x', im3_16_cores_radius, im3_16_cores, '-x', im4_16_cores_radius, im4_16_cores, '-x');
legend('im1', 'im2', 'im3', 'im4', 'Location', 'NorthWest');
xlabel('Radius');
ylabel('Run time');
title('Averaging filter run on 16 cores');

%./matlab_format.py slurm-604602.out 8_cores
im1_8_cores = [0.0298309 0.014122 0.0487628 0.09234];
im1_8_cores_radius = [1 10 50 100];
im2_8_cores = [0.00946903 0.0256338 0.101179 0.217682];
im2_8_cores_radius = [1 10 50 100];
im3_8_cores = [0.016274 0.0463929 0.177609 0.344069];
im3_8_cores_radius = [1 10 50 100];
im4_8_cores = [0.075248 0.211843 0.823263 1.63828];
im4_8_cores_radius = [1 10 50 100];
figure(3)
plot(im1_8_cores_radius, im1_8_cores, '-x', im2_8_cores_radius, im2_8_cores, '-x', im3_8_cores_radius, im3_8_cores, '-x', im4_8_cores_radius, im4_8_cores, '-x');
legend('im1', 'im2', 'im3', 'im4', 'Location', 'NorthWest');
xlabel('Radius');
ylabel('Run time');
title('Averaging filter run on 8 cores');


%./matlab_format.py slurm-521808.out 1_thread
im1_1_thread = [0.0470434 0.0796472 0.333709 0.631312];
im1_1_thread_radius = [5 10 50 100];
im2_1_thread = [0.095533 0.161768 0.684766 1.31056];
im2_1_thread_radius = [5 10 50 100];
im3_1_thread = [0.176316 0.297651 1.25851 2.43034];
im3_1_thread_radius = [5 10 50 100];
im4_1_thread = [0.826119 1.39829 5.9504 11.6048];
im4_1_thread_radius = [5 10 50 100];
figure(4)
plot(im1_1_thread_radius, im1_1_thread, '-x', im2_1_thread_radius, im2_1_thread, '-x', im3_1_thread_radius, im3_1_thread, '-x', im4_1_thread_radius, im4_1_thread, '-x');
legend('im1', 'im2', 'im3', 'im4', 'Location', 'NorthWest');
xlabel('Radius');
ylabel('Run time');
title('Averaging filter run with 1 thread');


%./matlab_format.py slurm-521810.out 2_threads
im1_2_threads = [0.025016 0.0427838 0.16885 0.318079];
im1_2_threads_radius = [5 10 50 100];
im2_2_threads = [0.0504859 0.0846524 0.34507 0.663202];
im2_2_threads_radius = [5 10 50 100];
im3_2_threads = [0.0924769 0.1519 0.634045 1.23104];
im3_2_threads_radius = [5 10 50 100];
im4_2_threads = [0.413716 0.702383 2.9809 5.81111];
im4_2_threads_radius = [5 10 50 100];
figure(5)
plot(im1_2_threads_radius, im1_2_threads, '-x', im2_2_threads_radius, im2_2_threads, '-x', im3_2_threads_radius, im3_2_threads, '-x', im4_2_threads_radius, im4_2_threads, '-x');
legend('im1', 'im2', 'im3', 'im4', 'Location', 'NorthWest');
xlabel('Radius');
ylabel('Run time');
title('Averaging filter run with 2 threads');

%./matlab_format.py slurm-521815.out 4_threads
im1_4_threads = [0.0140645 0.0225147 0.0872207 0.161822];
im1_4_threads_radius = [5 10 50 100];
im2_4_threads = [0.0274257 0.0445793 0.17969 0.336393];
im2_4_threads_radius = [5 10 50 100];
im3_4_threads = [0.0508857 0.0817186 0.324281 0.616372];
im3_4_threads_radius = [5 10 50 100];
im4_4_threads = [0.220054 0.360046 1.51232 2.94226];
im4_4_threads_radius = [5 10 50 100];
figure(5);
plot(im1_4_threads_radius, im1_4_threads, '-x', im2_4_threads_radius, im2_4_threads, '-x', im3_4_threads_radius, im3_4_threads, '-x', im4_4_threads_radius, im4_4_threads, '-x');
legend('im1', 'im2', 'im3', 'im4', 'Location', 'NorthWest');
xlabel('Radius');
ylabel('Run time');
title('Averaging filter run with 4 threads');

%./matlab_format.py slurm-521817.out 8_threads
im1_8_threads = [0.0104665 0.0146436 0.0481916 0.0860901];
im1_8_threads_radius = [5 10 50 100];
im2_8_threads = [0.0199493 0.0295607 0.0984862 0.183738];
im2_8_threads_radius = [5 10 50 100];
im3_8_threads = [0.0363348 0.0511522 0.178057 0.330515];
im3_8_threads_radius = [5 10 50 100];
im4_8_threads = [0.127996 0.206565 0.79577 1.52823];
im4_8_threads_radius = [5 10 50 100];
figure(6);
plot(im1_8_threads_radius, im1_8_threads, '-x', im2_8_threads_radius, im2_8_threads, '-x', im3_8_threads_radius, im3_8_threads, '-x', im4_8_threads_radius, im4_8_threads, '-x');
legend('im1', 'im2', 'im3', 'im4', 'Location', 'NorthWest');
xlabel('Radius');
ylabel('Run time');
title('Averaging filter run with 8 threads');

%./matlab_format.py slurm-521819.out 16_threads
im1_16_threads = [0.0114095 0.012666 0.0295881 0.0505437];
im1_16_threads_radius = [5 10 50 100];
im2_16_threads = [0.0220951 0.026521 0.0615479 0.10425];
im2_16_threads_radius = [5 10 50 100];
im3_16_threads = [0.0428284 0.0474971 0.110336 0.191794];
im3_16_threads_radius = [5 10 50 100];
im4_16_threads = [0.104575 0.143931 0.458426 0.851565];
im4_16_threads_radius = [5 10 50 100];
figure(7);
plot(im1_16_threads_radius, im1_16_threads, '-x', im2_16_threads_radius, im2_16_threads, '-x', im3_16_threads_radius, im3_16_threads, '-x', im4_16_threads_radius, im4_16_threads, '-x');
legend('im1', 'im2', 'im3', 'im4', 'Location', 'NorthWest');
xlabel('Radius');
ylabel('Run time');
title('Averaging filter run with 16 threads');

%%
figure(8)
plot(im4_8_cores_radius, im4_8_cores, '-x', im4_16_cores_radius, im4_16_cores, '-x', im4_32_cores_radius, im4_32_cores, '-x');
legend('8 cores', '16 cores', '32 cores', 'Location', 'NorthWest');
xlabel('Radius');
ylabel('Run time');
title('Averaging filter on im4.ppm different # cores');

%%
figure(9)
plot(im4_1_thread_radius, im4_1_thread, '-x', im4_2_threads_radius, im4_2_threads, '-x', im4_4_threads_radius, im4_4_threads, '-x', im4_8_threads_radius, im4_8_threads, '-x', im4_16_threads_radius, im4_16_threads, '-x');
legend('1 thread', '2 thread', '4 thread', '8 thread', '16 thread', 'Location', 'NorthWest');
xlabel('Radius');
ylabel('Run time');
title('Averaging filter on im4.ppm different # threads');

%%
mpi_thres_8_cores = [0.00768995 0.00541592 0.00714207 0.0279419];
mpi_thres_16_cores = [0.0297282 0.0194161 0.0152361 0.0306139];
mpi_thres_1_core = [0.00366712 0.00708914 0.01314 0.062237];
mpi_thres_4_cores = [0.002702 0.00367713 0.00692105 0.0318539];
im = [1 2 3 4];
%cores = [1 4 8 16];
%mpi_im2_core_var = [mpi_thres_1_core(2) mpi_thres_4_cores(2) mpi_thres_8_core(2) mpi_thres_16_cores(2)];
im = [1 2 3 4];
figure(10)
plot(im, mpi_thres_1_core, '-x', im, mpi_thres_4_cores, '-x', im, mpi_thres_8_cores, '-x', im, mpi_thres_16_cores, '-x');
%plot(cores, mpi_im2_core_var, '-x', cores, mpi_im2_core_var, '-x', cores, mpi_im2_core_var, '-x', cores, mpi_im2_core_var, '-x');
legend('1 core', '4 cores', '8 cores', '16 cores', 'Location', 'NorthWest');
xlabel('Image');
ylabel('Run time');
title('Threshold filter with mpi on different images');

%%


%%
thres_1_thread = [0.0025102 0.00499036 0.00936788 0.0392];
thres_4_threads = [0.000823773 0.00137079 0.00245882 0.0106272];
thres_8_threads = [0.00057446 0.000958058 0.00143139 0.00550633];
thres_16_threads = [0.000768185 0.000902217 0.00116739 0.00329923];
im = [1 2 3 4];
figure(11)
plot(im, thres_1_thread, '-x', im, thres_4_threads, '-x', im, thres_8_threads, '-x', im, thres_16_threads, '-x');
legend('1 threads', '4 threads', '8 threads', '16 threads', 'Location', 'NorthWest');
xlabel('Image');
ylabel('Run time');
title('Threshold filter with pthreads on different images');


%%
plot(im, im3_32_cores, '-x', im, im2_16_cores, '-x', im, im1_8cores, '-x');
legend('im3 32 cores', 'im2 16 cores', 'im1 8 cores', 'Location', 'NorthWest');
xlabel('Radius');
ylabel('Run time');
title('Changing computional complexity and number of processes');
