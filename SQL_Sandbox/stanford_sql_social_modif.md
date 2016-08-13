#It's time for the seniors to graduate. Remove all 12th graders from Highschooler. 

delete from Highschooler
where grade = 12;


#If two students A and B are friends, and A likes B but not vice-versa, remove the Likes tuple. 

delete from Likes
where Likes.ID1 in
(select distinct Friend.ID1
from Friend
where Friend.ID2 in (select F1.ID2 from Friend F1 where F1.ID1 = Friend.ID1) and
Friend.ID1 in (select F2.ID2 from Friend F2 where F2.ID1 = Friend.ID2) and
Friend.ID2 in (select L1.ID2 from Likes L1 where L1.ID1 = Friend.ID1) and not
Friend.ID1 in (select L2.ID2 from Likes L2 where L2.ID1 = Friend.ID2))

#For all cases where A is friends with B, and B is friends with C, add a new friendship for the pair A and C. Do not add duplicate friendships, friendships that already exist, or friendships with oneself. (This one is a bit challenging; congratulations if you get it right.) 

insert into Friend 
select distinct F.ID1, FB.ID2
from Friend F
join Friend FB on FB.ID1 = F.ID2
where FB.ID2 not in (select FA.ID2 from Friend FA where FA.ID1 = F.ID1) 
and not exists 
(select FD.ID1, FD.ID2 from Friend FD 
where (FD.ID1 = F.ID1 and FD.ID2 = FB.ID2) or (FD.ID2 = F.ID1 and FD.ID1 = FB.ID2))
and FB.ID2 <> F.ID1