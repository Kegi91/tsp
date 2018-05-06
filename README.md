# TSP

Travelling Salesman Problem with simulated annealing. Solve and visualize the optimal route
through the 20 biggest cities in Finland with commands:
```
cd src/
make run
```

This reads the file input/coords.txt and tries to find the optimal route by running 10 
individual simulations with 1e6 MCMC steps and initial temperature of T=2000.
