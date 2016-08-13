#Find the names of all students who are friends with someone named Gabriel. 

select name
from Highschooler
join
(select Friend.ID1 as id
from Friend
join Highschooler on Friend.ID2 = Highschooler.ID
where (Friend.id2 = 1689 or Friend.id2 = 1911)) as t
on Highschooler.ID = t.id

#For every student who likes someone 2 or more grades younger than themselves, return that student's name and grade, and the name and grade of the student they like.

select H1.name, H1.grade, H2.name, H2.grade
from Likes
join Highschooler H1 on H1.ID = Likes.ID1
join Highschooler H2 on H2.ID = Likes.ID2
where (H1.grade- H2.grade>=2)

#For every pair of students who both like each other, return the name and grade of both students. Include each pair only once, with the two names in alphabetical order. 

select H1.name, H1.grade, H2.name, H2.grade
from Likes
join Highschooler H1 on H1.ID = Likes.ID1
join Highschooler H2 on H2.ID = Likes.ID2
where Likes.ID2 in (select L1.ID2 from Likes L1 where L1.ID1 = Likes.ID1) and
Likes.ID1 in (select L2.ID2 from Likes L2 where L2.ID1 = Likes.ID2) and H1.name < H2.name

#Find all students who do not appear in the Likes table (as a student who likes or is liked) and return their names and grades. Sort by grade, then by name within each grade. 

select name, grade
from Highschooler 
where (ID not in (select ID1 from Likes L1 ) and ID not in (select ID2 from Likes L2))
ORDER BY grade, name

#For every situation where student A likes student B, but we have no information about whom B likes (that is, B does not appear as an ID1 in the Likes table), return A and B's names and grades. 

select H1.name, H1.grade, H2.Name, H2.grade
from Likes
join Highschooler H1 on H1.ID = Likes.ID1
join Highschooler H2 on H2.ID = Likes.ID2
where (ID2 not in (select L1.ID1 from Likes L1))

#Find names and grades of students who only have friends in the same grade. Return the result sorted by grade, then by name within each grade. 

select H3.name, H3.grade
from
(select ID1 as i2, count(ID1) as c2
from Friend
group by ID1) as v
join
(select ID1 as i1, count(ID1) as c1
from Friend
join Highschooler H1 on H1.ID = Friend.ID1
join Highschooler H2 on H2.ID = Friend.ID2
where H1.grade = H2.grade
group by ID1) as t
on v.i2 = t.i1
join Highschooler H3 on H3.ID = v.i2
where v.c2 = t.C1
ORDER BY H3.grade, H3.name

#For each student A who likes a student B where the two are not friends, find if they have a friend C in common (who can introduce them!). For all such trios, return the name and grade of A, B, and C. 


select h1n, h1g, h2n, h2g, H3.name, H3.Grade
from
(select Likes.ID1 as l1, Likes.ID2 as l2, H1.name as h1n, H1.grade as h1g, H2.name as h2n, H2.grade as h2g
from Likes
join Highschooler H1 on H1.ID = Likes.ID1
join Highschooler H2 on H2.ID = Likes.ID2
where Likes.ID2 not in (select Friend.ID2 from Friend where Friend.ID1 = Likes.ID1)) as t
join Friend F1 on F1.ID1 = t.l1
join Friend F2 on F2.ID1 = t.l2
join Highschooler H3 on H3.ID = F1.ID2
where F1.ID2 = F2.ID2

#Find the difference between the number of students in the school and the number of different first names. 

select t1.c1 -t2.c2
from
(SELECT COUNT(DISTINCT ID) as c1 from Highschooler) as t1,
(SELECT COUNT(DISTINCT name) as c2 from Highschooler) as t2

#Find the name and grade of all students who are liked by more than one other student. 

select Highschooler.name, Highschooler.grade
from Likes
join Highschooler on Likes.ID2 = Highschooler.ID
group by Likes.ID2
having count(ID2)>1