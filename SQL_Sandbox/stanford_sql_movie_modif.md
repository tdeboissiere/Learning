#Add the reviewer Roger Ebert to your database, with an rID of 209. 

insert into Reviewer
values (209, 'Roger Ebert')

#Insert 5-star ratings by James Cameron for all movies in the database. Leave the review date as NULL. 

insert into Rating
select 207, Movie.mID, 5, null from Movie

#For all movies that have an average rating of 4 stars or higher, add 25 to the release year. (Update the existing tuples; don't insert new tuples.) 

update Movie set year = year+25
where Movie.mID in 
(select R.mID from Rating R group by R.mID having avg(R.stars)>=4)

#Remove all ratings where the movie's year is before 1970 or after 2000, and the rating is fewer than 4 stars.

delete from Rating
where mID in (select M.mID from Movie M where (M.year<1970 or M.year>2000)) and stars <4