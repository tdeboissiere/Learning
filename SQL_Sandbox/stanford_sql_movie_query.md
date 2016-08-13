#Find the titles of all movies directed by Steven Spielberg. 

select title
from Movie
where Movie.Director = 'Steven Spielberg';

#Find all years that have a movie that received a rating of 4 or 5, and sort them in increasing order.
SELECT DISTINCT year 
FROM Movie 
INNER JOIN Rating on Movie.mID = Rating.mID 
WHERE Rating.stars>=4 ORDER BY year ASC

#Find the titles of all movies that have no ratings. 
SELECT Movie.title
FROM Movie
WHERE Movie.title NOT IN (SELECT Movie.title FROM Rating LEFT JOIN Movie ON Rating.mID = Movie.mID)
;

#Some reviewers didn't provide a date with their rating. Find the names of all reviewers who have ratings with a NULL value for the date. 
SELECT DISTINCT Reviewer.name 
FROM Reviewer 
INNER JOIN Rating on Reviewer.rID = Rating.rID 
where Rating.ratingdate IS NULL

#Write a query to return the ratings data in a more readable format: reviewer name, movie title, stars, and ratingDate. Also, sort the data, first by reviewer name, then by movie title, and lastly by number of stars. 
SELECT Reviewer.name as "reviewer name", Movie.title as "movie title", Rating.stars as stars, Rating.ratingDate as ratingDate
FROM Reviewer JOIN Rating
ON Reviewer.rID = Rating.rID
JOIN Movie
ON Rating.mID = Movie.mID
ORDER BY Reviewer.name, Movie.title, Rating.stars

#For all cases where the same reviewer rated the same movie twice and gave it a higher rating the second time, return the reviewer's name and the title of the movie. 
select Reviewer.name, Movie.title
from Rating
join Movie on Movie.mID = Rating.mID
join Reviewer on Reviewer.rID = Rating.rID
group by Rating.mID, Rating.rID
HAVING (stars=MAX(stars) and ratingDate =MAX(ratingDate) and count(Rating.rID)=2)

#For each movie that has at least one rating, find the highest number of stars that movie received. Return the movie title and number of stars. Sort by movie title. 
select Movie.title, t.maxstar
from (select Rating.mID as RmID, max(Rating.stars) as maxstar from Rating group by Rating.mID) as t
 join Movie on Movie.mID = RmID
ORDER BY Movie.title
;

#For each movie, return the title and the 'rating spread', that is, the difference between highest and lowest ratings given to that movie. Sort by rating spread from highest to lowest, then by movie title. 
select Movie.title, t.spread
from (select Rating.mID as RmID, max(Rating.stars) - min(Rating.stars) as spread from Rating group by Rating.mID) as t
 join Movie on Movie.mID = RmID
ORDER BY t.spread DESC, Movie.title
;

#Find the difference between the average rating of movies released before 1980 and the average rating of movies released after 1980. (Make sure to calculate the average rating for each movie, then the average of those averages for movies before 1980 and movies after. Don't just calculate the overall average rating before and after 1980.) 
select avg(ta.ratinga) - avg(tb.ratingb)
from 
(select avg(stars) as ratinga
from Rating
join Movie
where (Rating.mID = Movie.mID and Movie.year<1980)
group by Rating.mID) as ta,
(select avg(stars) as ratingb
from Rating
join Movie
where (Rating.mID = Movie.mID and Movie.year>1980)
group by Rating.mID) as tb