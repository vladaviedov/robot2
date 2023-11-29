using DataFrames
using CSV
using Statistics

const cam_center_x = 1280 / 2
const cam_center_y = 720 / 2

function target_to_delta(target)
	return (target[1] - cam_center_x, target[2] - cam_center_y)
end

function error_rating(target_delta, computed_delta)
	# x-component more important
	local x_diff = target_delta[1] - computed_delta[1]
	local x_rating = 0
	if x_diff < 200
		x_rating = x_diff ^ 2
	else
		x_rating = 200 ^ 2 + (x_diff - 200) ^ 3
	end

	# y-component less important
	local y_diff = target_delta[2] - computed_delta[2]
	local y_rating = 0
	if y_diff < 200
		y_rating = y_diff ^ 2 / 3
	else
		y_rating = (200 ^ 2 + (y_diff - 200) ^ 3) / 3
	end

	return x_rating + y_rating
end

function error_cutoff(rating)
	return rating < 10000
end

targets = [
	(0, 0)
	(724, 216)
	(666, 185)
	(651, 185)
	(731, 111)
	(311, 307)
	(297, 287)
	(90, 410)
	(144, 395)
	(749, 60)
	(715, 67)
	(951, 169)
	(1083, 204)
	(1101, 254)
	(1096, 266)
	(551, 162)
	(1038, 380)
	(451, 49)
	(657, 214)
	(820, 87)
]

frames = []
for i in 2:20
	println("Processing image ", i)

	local target_delta = target_to_delta(targets[i])
	println(target_delta)

	local data = DataFrame(CSV.File(string("../data/image", i, ".csv")))
	data[!, "error_rating"] = map((x, y) -> error_rating(target_delta, (x, y)), data.delta_x, data.delta_y)

	push!(frames, data)
end

combined = frames[1]
for i in 2:19
	global combined = vcat(combined, frames[i])
end

grouped = groupby(combined, [:white, :black, :kernel, :iter, :max_area, :min_size])
filtered = filter(:white => w -> length(w) == 19, grouped)
recombined = combine(filtered, :error_rating => mean)
sort!(recombined, :error_rating_mean)
display(first(recombined, 200))
