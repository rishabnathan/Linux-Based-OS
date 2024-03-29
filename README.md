Overview
-----
This is the final project for my Computer Systems (ECE 391) at UIUC. This project is a fully functional Linux-like operating system that supports task scheduling and switching, virtual memory management, a read-only file system, paging, interrupts and exceptions, and device drivers for keyboard input, terminal output, and a Real-Time Clock.

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes. 

### Prerequisites

You must have the QEMU emulator already installed 

```
qemu-system-i386 -hda "student-distrib/mp3.img" -m 256 -name linuxOS
```

### 

### Compiling

Build the kernel using `make` within the `student-distrib` folder using a i386-elf-gcc toolchain or the  gcc from the ECE391 devel. 

Create the bootable image, invoke `makeos.sh` in the root directory. You must have root priviliges and your system must include a mount utility with ext2 support.

The mp3.img that results is a raw disk image. 

### Installing

To run the OS in QEMU, invoke...

```
qemu-system-i386 -hda "student-distrib/mp3.img" -m 256 -name linuxOS
```

## Running the tests

Unit tests can be run by uncommenting the `#define RUN_TESTS` line in kernel.c and enabling specific tests by uncommenting the specific tests in the `launch_tests()` function in tests.c. 

Tests will print output onto Terminal 1 and return a *PASS* or *FAIL*

## Demo

A short demo of the OS demonstrates round-robin scheduling on a uniprocessor system, as well as multiple terminal and file-system functionality. Low frame rate is due to rendering of gif and not seen during OS use.

![demo](OS_demo.gif)

ACADEMIC INTEGRITY
-----
Please refer your student code and the academic-integrity.md file for academic integrity requirements. You may use code from this project freely under the license terms, but it is **your own responsibility** to ensure that such use meets the requirements set by your course and department. I am not responsible for any academic integrity violations for use of other students copying any portion of this code.

Please review the University of Illinois Student Code before starting,
particularly all subsections of Article 1, Part 4 Academic Integrity and Procedure [here](http://studentcode.illinois.edu/article1_part4_1-401.html).
**§ 1‑402 Academic Integrity Infractions**
(a).	Cheating. No student shall use or attempt to use in any academic exercise materials, information, study aids, or electronic data that the student knows or should know is unauthorized. Instructors are strongly encouraged to make in advance a clear statement of their policies and procedures concerning the use of shared study aids, examination files, and related materials and forms of assistance. Such advance notification is especially important in the case of take-home examinations. During any examination, students should assume that external assistance (e.g., books, notes, calculators, and communications with others) is prohibited unless specifically authorized by the Instructor. A violation of this section includes but is not limited to:
(1)	Allowing others to conduct research or prepare any work for a student without prior authorization from the Instructor, including using the services of commercial term paper companies. 
(2)	Submitting substantial portions of the same academic work for credit more than once or by more than one student without authorization from the Instructors to whom the work is being submitted. 
(3) Working with another person without authorization to satisfy an individual assignment.
(b) Plagiarism. No student shall represent the words, work, or ideas of another as his or her own in any academic endeavor. A violation of this section includes but is not limited to:
(1)	Copying: Submitting the work of another as one’s own. 
(2)	Direct Quotation: Every direct quotation must be identified by quotation marks or by appropriate indentation and must be promptly cited. Proper citation style for many academic departments is outlined in such manuals as the MLA Handbook or K.L. Turabian’s A Manual for Writers of Term Papers, Theses and Dissertations. These and similar publications are available in the University bookstore or library. The actual source from which cited information was obtained should be acknowledged.
(3)	Paraphrase: Prompt acknowledgment is required when material from another source is paraphrased or summarized in whole or in part. This is true even if the student’s words differ substantially from those of the source. A citation acknowledging only a directly quoted statement does not suffice as an acknowledgment of any preceding or succeeding paraphrased material. 
(4)	Borrowed Facts or Information: Information obtained in one’s reading or research that is not common knowledge must be acknowledged. Examples of common knowledge might include the names of leaders of prominent nations, basic scientific laws, etc. Materials that contribute only to one’s general understanding of the subject may be acknowledged in a bibliography and need not be immediately cited. One citation is usually sufficient to acknowledge indebtedness when a number of connected sentences in the paper draw their special information from one source.
(c) Fabrication. No student shall falsify or invent any information or citation in an academic endeavor. A violation of this section includes but is not limited to:
(1)	Using invented information in any laboratory experiment or other academic endeavor without notice to and authorization from the Instructor or examiner. It would be improper, for example, to analyze one sample in an experiment and covertly invent data based on that single experiment for several more required analyses. 
(2)	Altering the answers given for an exam after the examination has been graded. 
(3)	Providing false or misleading information for the purpose of gaining an academic advantage.
(d)	Facilitating Infractions of Academic Integrity. No student shall help or attempt to help another to commit an infraction of academic integrity, where one knows or should know that through one’s acts or omissions such an infraction may be facilitated. A violation of this section includes but is not limited to:
(1)	Allowing another to copy from one’s work. 
(2)	Taking an exam by proxy for someone else. This is an infraction of academic integrity on the part of both the student enrolled in the course and the proxy or substitute. 
(3)	Removing an examination or quiz from a classroom, faculty office, or other facility without authorization.
(e)	Bribes, Favors, and Threats. No student shall bribe or attempt to bribe, promise favors to or make threats against any person with the intent to affect a record of a grade or evaluation of academic performance. This includes conspiracy with another person who then takes the action on behalf of the student.
(f)	Academic Interference. No student shall tamper with, alter, circumvent, or destroy any educational material or resource in a manner that deprives any other student of fair access or reasonable use of that material or resource. 
(1)	Educational resources include but are not limited to computer facilities, electronic data, required/reserved readings, reference works, or other library materials. 
(2)	Academic interference also includes acts in which the student committing the infraction personally benefits from the interference, regardless of the effect on other students.

LEGAL
-----
Permission to use, copy, modify, and distribute this software and its
documentation for any purpose, without fee, and without written agreement is
hereby granted, provided that the above copyright notice and the following
two paragraphs appear in all copies of this software.
IN NO EVENT SHALL THE AUTHOR OR THE UNIVERSITY OF ILLINOIS BE LIABLE TO
ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
DAMAGES ARISING OUT  OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION,
EVEN IF THE AUTHOR AND/OR THE UNIVERSITY OF ILLINOIS HAS BEEN ADVISED
OF THE POSSIBILITY OF SUCH DAMAGE.

## License

This project is licensed under the MIT License (see the [LICENSE.md](LICENSE.md) file for details). However, some submodule components are licensed under the GPL. Please refer to the specific submodules for their detailed license texts.
