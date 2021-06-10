bar(task4replacement);
ax = gca;
ax.XAxis.Limits = [0.5 4.5];
ax.XAxis.TickLabels = {'FIFO','LRU','random'};
legend('fibonacci','matmul','memcopy','pi','whetstone')
xlabel('replacement policy')
ylabel('miss rate')
title('Effect of Replacement Policy on Miss Rate')
grid on;