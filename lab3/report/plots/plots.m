threads = [1 2 4 8 16];
size = [1000 1414 2000 2828 4000 10000];

n_1000_time = [0.860502 0.464044 0.308580 0.174112 0.216558];
n_1000_flops_t = [3242.293583 3000.792351 2256.303586 1999.430418 803.768563];
n_1000_flops = [3242.293583 6001.584702 9025.214345 15995.443346 12860.297004];

n_1414_time = [1.860594 0.969823 0.601488 0.321270 0.250526];
n_1414_flops_t = [2998.136428 2870.790470 2314.392350 2166.525875 1389.156972];
n_1414_flops = [2998.136428 5741.580939 9257.569398 17332.206999 22226.511550];

n_2000_time = [4.287069 2.249580 1.142861 0.646555 0.429628];
n_2000_flops_t = [2603.177350 2476.017529 2436.866447 2153.722549 1620.588401];
n_2000_flops = [2603.177350 4952.035058 9747.465788 17229.780388 25929.414418];

n_2828_time = [10.502358 5.285332 2.949943 2.097788 2.347611];
n_2828_flops_t = [2124.595171 2107.083525 1887.601838 1327.188050 592.977081];
n_2828_flops = [2124.595171 4214.167049 7550.407351 10617.504403 9487.633291];

n_4000_time = [17.585805 10.759049 5.916140 4.816525 5.188635];
n_4000_flops_t = [2538.411077 2070.814959 1882.984445 1156.435375 536.750008];
n_4000_flops = [2538.411077 4141.629918 7531.937780 9251.482996 8588.000124];

n_10000_time = [139.106232 69.882628 39.582568 29.723183 31.368346];
n_10000_flops_t = [2005.661331 1992.626947 1758.981472 1171.223893 554.898592];
n_10000_flops = [2005.661331 3985.253893 7035.925889 9369.791146 8878.377468];

flops = [n_1000_flops; n_1414_flops; n_2000_flops; n_2828_flops; n_4000_flops; n_10000_flops]';
flops_t = [n_1000_flops_t; n_1414_flops_t; n_2000_flops_t; n_2828_flops_t; n_4000_flops_t; n_10000_flops_t]';

figure(1);
plot(threads, n_1000_time, 'b-x', threads, n_1414_time, 'r-x', threads, n_2000_time, 'g-x', threads, n_2828_time, 'y-x', threads, n_4000_time, 'c-x');
xlabel('Threads');
ylabel('Execution time [s]')
legend('N = 1000', 'N = 1414', 'N = 2000', 'N = 2828', 'N = 4000');

figure(2);
plot(threads, n_1000_flops, 'b-x', threads, n_1414_flops, 'r-x', threads, n_2000_flops, 'g-x', threads, n_2828_flops, 'y-x', threads, n_4000_flops, 'c-x');
xlabel('Threads');
ylabel('Total MFLOPS')
legend('N = 1000', 'N = 1414', 'N = 2000', 'N = 2828', 'N = 4000', 'Location', 'Northwest');

figure(3);
plot(size, flops(1,:), 'b-x', size, flops(2,:), 'r-x', size, flops(3,:), 'g-x', size, flops(4,:), 'y-x', size, flops(5,:), 'c-x');
xlabel('Problem Size (N)');
ylabel('Total MFLOPS')
legend('1', '2', '4', '8', '16', 'Location', 'Northeast');

figure(4);
plot(size, flops_t(1,:), 'b-x', size, flops_t(2,:), 'r-x', size, flops_t(3,:), 'g-x', size, flops_t(4,:), 'y-x', size, flops_t(5,:), 'c-x');
xlabel('Problem Size (N)');
ylabel('MFLOPS per thread');
ylim([0 3500]);
legend('1', '2', '4', '8', '16', 'Location', 'Northeast');

figure(5);
plot(threads,[n_1000_time(1) n_1414_time(2) n_2000_time(3) n_2828_time(4) n_4000_time(5)], 'b-x');
xlabel('Problem size / Threads');
ylabel('Execution time [s]');

