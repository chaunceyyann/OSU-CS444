## Concurrency 4: The Barbershop Problem

### Due: 23:59:59 17 November 2016

</center>

* * *

The purpose of these concurrent programming exercises is to hone your skills in thinking in parallel. This is a very important skill, to the point where many companies view it as being as fundamental as basic algebra or being able to write code. You will be writing a solution to the problem in class, without the use of computers. This will be submitted prior to leaving the class today. Make sure you keep a copy though, as you will implement your solution over the course of the next week.

## Problem 1

Consider a sharable resource with the following characteristics:

1.  As long as there are fewer than three processes using the resource, new processes can start using it right away.
2.  Once there are three processes using the resource, all three must leave before any new processes can begin using it.

Implement a mutual exclusion solution that meets the above constraints.

* * *

## Problem 2

A barbershop consists of a waiting room with _n_ chairs, and the barber room containing the barber chair. If there are no customers to be served, the barber goes to sleep. If a customer enters the barbershop and all chairs are occupied, then the customer leaves the shop. If the barber is busy, but chairs are available, then the customer sits in one of the free chairs. If the barber is asleep, the customer wakes up the barber. Write a program to coordinate the barber and the customers.

Some useful constraints:

*   Customers invoke <tt>get_hair_cut</tt> when sitting in the barber chair.
*   If the shop is full, a customer exits the shop.
*   The barber thread invokes <tt>cut_hair</tt>.
*   <tt>cut_hair</tt> and <tt>get_hair_cut</tt> should always be executing concurrently, for the same duration.
*   Your solution should be valid for any number of chairs.

Write concurrent code for customers and barbers that implements a solution to the above constraints.

* * *
