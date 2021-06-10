load('task1data.mat');

figure();
h = plot(task1sizefibonacci);
set(h,{'Marker'},{'+';'s';'o';'*'})
ax = gca;
ax.XAxis.TickValues = [1,2,3,4,5,6,7,8];
ax.XAxis.TickLabels = {'1K','2K','4K','8K','16K','32K','64K','128K'};
xlim([1 10])
title('fibonacci benchmark')
xlabel('number of sets')
ylabel('miss rate')
legend('8B','16B','32B','64B')
grid on;

figure();
h = plot(task1sizematmul);
set(h,{'Marker'},{'+';'s';'o';'*'})
ax = gca;
ax.XAxis.TickValues = [1,2,3,4,5,6,7,8];
ax.XAxis.TickLabels = {'1K','2K','4K','8K','16K','32K','64K','128K'};
xlim([1 10])
title('matmul benchmark')
xlabel('number of sets')
ylabel('miss rate')
legend('8B','16B','32B','64B')
grid on;

figure();
h = plot(task1sizememcopy);
set(h,{'Marker'},{'+';'s';'o';'*'})
ax = gca;
ax.XAxis.TickValues = [1,2,3,4,5,6,7,8];
ax.XAxis.TickLabels = {'1K','2K','4K','8K','16K','32K','64K','128K'};
xlim([1 10])
title('memcopy benchmark')
xlabel('number of sets')
ylabel('miss rate')
legend('8B','16B','32B','64B')
grid on;

figure();
h = plot(task1sizepi);
set(h,{'Marker'},{'+';'s';'o';'*'})
ax = gca;
ax.XAxis.TickValues = [1,2,3,4,5,6,7,8];
ax.XAxis.TickLabels = {'1K','2K','4K','8K','16K','32K','64K','128K'};
xlim([1 10])
title('pi benchmark')
xlabel('number of sets')
ylabel('miss rate')
legend('8B','16B','32B','64B')
grid on;
ylim([0.4e-3 3.5e-3])

figure();
h = plot(task1sizewhetstone);
set(h,{'Marker'},{'+';'s';'o';'*'})
ax = gca;
ax.XAxis.TickValues = [1,2,3,4,5,6,7,8];
ax.XAxis.TickLabels = {'1K','2K','4K','8K','16K','32K','64K','128K'};
xlim([1 10])
title('whetstone benchmark')
xlabel('number of sets')
ylabel('miss rate')
legend('8B','16B','32B','64B')
grid on;
ylim([1.8e-4 16.2e-4])