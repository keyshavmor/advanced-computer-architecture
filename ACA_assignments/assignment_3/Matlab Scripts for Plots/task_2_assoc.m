figure();
plot(fibonacci,'o-r');
hold on;
plot(matmul,'o-m');
plot(memcopy,'o-b');
plot(pi1,'o-g');
plot(whetstone,'o-c');
grid on;
ax = gca;
ax.XAxis.Limits = [0.9 5.1];
ax.XAxis.TickValues = [1,2,3,4,5];
ax.XAxis.TickLabels = {'1-way','2-way','4-way','8-way','16-way'};
legend('fibonacci','matmul','memcopy','pi','whetstone','Location','NorthEast')
xlabel('associativity')
ylabel('miss rate')
title('Effect of Associativity on Miss Rate')