# “JemHub” GitHub Interpretation #

Elvis Wolcott, Jonas Kazlauskas, Miles Mezaki

### Project Objectives and Expectations ###

Our goal is to make a git-like version control system. We hope to extend this MVP to add a way for multiple people to work on a repository, similar to github. Our lower bound is to create a software that allows us to create repositories and create commits on that repository, saving progress along the way. Our stretch goals are to add more advanced features in git such as branches and merging. We also want to add a way to sync repositories across a network.

### Project Learning Goals ###

#### Jonas
I want to learn more about how git works and why it is such a widely used version control system. I also want to work on my overall C development skills such as creating make files, using libraries, and networking.
#### Miles
I want to learn more about how Git optimizes for storage space while still maintaining the existence of every previous version of a document. I would love to discover exactly how this is done. I also want to work with sockets and understand internet connections on a more fundamental level.
#### Elvis
I want to learn more about low level network programming and how to write robust networked programs that can deal with loss of connection and other errors. I also hope to build the skills necessary to effectively develop c software on a team and take advantage of git and make effectively to do so. I would like to improve my ability to test c programs thoroughly and especially consider how to do this best around networking code and distributed systems.

### Resources and Needs ### 

In addition to our initial resources:

1. https://ericsink.com/entries/time_space_tradeoffs.html
1. https://stackoverflow.com/questions/902314/writing-my-own-file-versioning-program

we have read and discussed a great deal more:

1. https://web.mit.edu/6.031/www/sp22/classes/05-version-control/
1. https://jwiegley.github.io/git-from-the-bottom-up/1. 6-Further-Reading.html
1. https://missing.csail.mit.edu/2020/version-control/
1. http://www.faqs.org/rfcs/rfc3284.html
1. https://www.raywenderlich.com/books/advanced-git/v1.0/chapters/1-how-does-git-actually-work#toc-chapter-005-anchor-001
1. https://mincong.io/2018/04/28/git-index/
1. https://shafiul.github.io//gitbook/7_the_git_index.html
1. https://www.designcise.com/web/tutorial/what-is-a-commit-object-in-git
1. http://git-scm.com/book/en/v2/Git-Internals-Git-Objects

Overall, we probably have found enough resources to sustain any questions we may need to answer.

We have created a simple program called JEM that can be called with 4 different arguments: init, add, commit, and checkout. For each command we have created an outline for the functions needed to complete the operation. We have also created a file called structs.h that contains the data structures we are using for the application. 


Below is a summary of the functionality needed and completed for each command:

  **Init**: Creates a repository called .jem to store version information. Needs to then create the initial commit based on the current directory state.

    
  **Add**: Adds the filenames passed to the command to a list of strings contained in the Index struct. Needs the ability to save the index in storage when completed. 

  
  **Commit**: Creates a Commit struct with the message, author, snapshot tree and parent. Much of the information here is a placeholder to begin. Needs a way to load the saved index and delete it, create a snap tree from the index, and load the last commit to set its head. Lastly it also needs to be able to save the commit in storage


  **Checkout**: This function currently prints “checkout”. This needs a way to load the last commit, and reconstruct the repository based on it.


We have been largely basing our design off of git’s approach, and are using OpenSSL to hash objects such as files and commits so that we can store persistent references to them. We have the ability to hash files, parse commands, and load/store data structures but still need to integrate it all together to finish out the commands for the MVP.


### Current Task Structure ###

#### Jonas
Finish init and add commands.


**Definition of Done**: In a new directory, we can init a repository and then create a file and add it to the index. Running init a second time should result in an error, and the index file should contain a single valid entry for the file created.
#### Miles
Finish commit command.


**Definition of Done**: After initializing a repository and adding a file, we can run commit. This should result in an empty index and the creation of a snapshot and commit in the .jem folder.
#### Elvis
Finish checkout command.


**Definition of Done**: After initializing and committing to a repository, make a change and commit again. Running the checkout command with the hash of the first commit should return the file to the starting state.
