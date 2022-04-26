# “JemHub” GitHub Interpretation #

Elvis Wolcott, Jonas Kazlauskas, Miles Mezaki

### Project Objectives and Expectations ###

Our goal is to make a git-like version control system. We hope to extend this MVP to add a way for multiple people to work on a repository, similar to github. Our lower bound is to create a software that allows us to create repositories and create commits on that repository, saving progress along the way. Our stretch goals are to add more advanced features in git such as branches and merging. We also want to add a way to sync repositories across a network.

### Project Learning Goals ###

Jonas
I want to learn more about how git works and why it is such a widely used version control system. I also want to work on my overall C development skills such as creating make files, using libraries, and networking.
Miles
I want to learn more about how Git optimizes for storage space while still maintaining the existence of every previous version of a document. I would love to discover exactly how this is done. I also want to work with sockets and understand internet connections on a more fundamental level.
Elvis
I want to learn more about low level network programming and how to write robust networked programs that can deal with loss of connection and other errors. I also hope to build the skills necessary to effectively develop c software on a team and take advantage of git and make effectively to do so. I would like to improve my ability to test c programs thoroughly and especially consider how to do this best around networking code and distributed systems.

### Resources and Needs ### 

In addition to our initial resources:

https://ericsink.com/entries/time_space_tradeoffs.html
https://stackoverflow.com/questions/902314/writing-my-own-file-versioning-program

we have read and discussed a great deal more:

https://web.mit.edu/6.031/www/sp22/classes/05-version-control/
https://jwiegley.github.io/git-from-the-bottom-up/6-Further-Reading.html
https://missing.csail.mit.edu/2020/version-control/
http://www.faqs.org/rfcs/rfc3284.html
https://www.raywenderlich.com/books/advanced-git/v1.0/chapters/1-how-does-git-actually-work#toc-chapter-005-anchor-001

Overall, we probably have found enough resources to sustain any questions we may need to answer.

### Current Task Structure ###

Miles: I am working on leveraging cryptography functions in the external openssl/crypto library to be able to hash objects and save them to disk.

Elvis:

Jonas: