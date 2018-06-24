# AIO 

![image](https://user-images.githubusercontent.com/22048950/39088894-46e43e94-45c3-11e8-838a-cae1fae978c2.png)

### **AIO** is a ultra short programming language which contains basic types and allows write code more faster for multi threading.

# Philosophy
### First, building, then launching:

First, the AIO interpreter, collects all the information about the script and builds aio_object

![image](https://user-images.githubusercontent.com/22048950/41033429-285a9a78-698f-11e8-8ffc-2fe4430863b6.png)

After build, the AIO interpreter, interprets invoked method in the AIOObjects.
Thus, you can with AIO interpreter invoke different tasks in new threads.

![image](https://user-images.githubusercontent.com/22048950/41034305-d4968408-6991-11e8-8f26-14977f2563a6.png)

# Features!
### Primitives:
![image](https://user-images.githubusercontent.com/22048950/39087940-a87666bc-45b1-11e8-8127-2f25fbad7829.png)
### Swapping:
![image](https://user-images.githubusercontent.com/22048950/39087949-ebf9251e-45b1-11e8-9849-c68f3977b434.png)
### Conditions:
![image](https://user-images.githubusercontent.com/22048950/39087960-3298fcd8-45b2-11e8-8f91-2adea8a36bdd.png)
### Loops:
![image](https://user-images.githubusercontent.com/22048950/39088126-526909ec-45b5-11e8-9982-a082b5e27e71.png)
### For:
![image](https://user-images.githubusercontent.com/22048950/39088142-8f4cb0ca-45b5-11e8-8eae-c17c99c435b6.png)
### While:
![image](https://user-images.githubusercontent.com/22048950/39088137-818e829c-45b5-11e8-8455-254f800eb869.png)
### For with condition:
![image](https://user-images.githubusercontent.com/22048950/39088164-eb687c5e-45b5-11e8-97b2-1696043a774f.png)
### Methods:
All methods have variable quantity of arguments and return variable amount of values:

![image](https://user-images.githubusercontent.com/22048950/39088183-44d5ff46-45b6-11e8-9560-4e2616dec18f.png)
### Examples:

 - Subtraction "a - b":

![image](https://user-images.githubusercontent.com/22048950/39088242-29480fc0-45b7-11e8-9ddd-31b3b7adec7e.png)

 - Count sum "a + b + c + .. + n":

![image](https://user-images.githubusercontent.com/22048950/39088452-f064c7f8-45ba-11e8-8232-00dc8878238e.png)

 - Shoter:

![image](https://user-images.githubusercontent.com/22048950/39088353-1c59c8c4-45b9-11e8-9579-4f6fbde44ad1.png)

- Very short:

![image](https://user-images.githubusercontent.com/22048950/39088379-8160d42e-45b9-11e8-8dc6-749a386ee02a.png)

- The shortest!

![image](https://user-images.githubusercontent.com/22048950/39088385-9101ac5a-45b9-11e8-969f-5803d91224d2.png)

### Arrays:
All arrays can keep different types of values:

![image](https://user-images.githubusercontent.com/22048950/39088429-548e2d38-45ba-11e8-9b01-5f6d4adbdd9a.png)

Also you can put array as return type:

![image](https://user-images.githubusercontent.com/22048950/39088463-35649202-45bb-11e8-882f-1cc1825d00a9.png)

### Special definitions:
Let’s have a look at @countBayesProb method which counts Bayes probability:

![image](https://user-images.githubusercontent.com/22048950/39088585-5c6f0074-45bd-11e8-859a-72eeca4e07e7.png)

This method very difficult to understand. Then ***declaration*** comes to help us:

### Declarations:

![image](https://user-images.githubusercontent.com/22048950/39088611-d8382e06-45bd-11e8-8fa8-9bef632b5818.png)

Declarations indicate the exact number of arguments:

![image](https://user-images.githubusercontent.com/22048950/39088667-cd9e10ae-45be-11e8-9618-0788fe02f4e7.png)

### Ants:

Ants is the most amazing part of AIO.

Foreword.
In 2000, aspect-oriented programming appeared. It was then possible to change the work of methods, functions, and procedures in runtime. 
One of the most popular ways is to proxy and create interceptors. Unfortunately, the most primitive proxy allows you to wrap methods only at the beginning and end, in order to do something more complicated to get to the bodies, you need to know how to do it. aspects work very slowly, since each method interceptor intercepts that too much effort on the performance.
So, you need to come up with an easy way to change the body of methods in the execution of the program.

AIO Ants:
In fact, the body of any method is an algorithm. Accordingly, it can be represented as a sequence of commands, where each command is essentially an implicit function that has arguments and a return value. And that if we consider every primitive operation, for example (a + b) as a microunion: give it a name: "addition", define the arguments (a, b) (for simplicity of the example there will be an integer), the return value is integer. Thus, the body of the method consists of elementary operations. Let's create links to these operations, and we will work with them.

With this approach, you can easily change the operation right inside the program process.