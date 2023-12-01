"""
@file main.jl
@brief Manipulate data to produce a ranking for CV 'models'
@note To run, include this file from the julia REPL
"""

using DataFrames
using CSV
using Statistics

# Camera center position
const cam_center_x = 1280 / 2
const cam_center_y = 720 / 2

# Convert absolute position to relative to center
function target_to_delta(target)
	return (target[1] - cam_center_x, target[2] - cam_center_y)
end

# Calculate error rating from camera guess
function error_rating(target_delta, computed_delta)
	# x-component more important
	local x_diff = target_delta[1] - computed_delta[1]
	local x_rating = 0
	
	if x_diff < 200
		x_rating = x_diff ^ 2
	else
		# Make it harder for large misses to count
		x_rating = 200 ^ 2 + (x_diff - 200) ^ 3
	end

	# y-component less important
	local y_diff = target_delta[2] - computed_delta[2]
	local y_rating = 0
	if y_diff < 200
		y_rating = y_diff ^ 2 / 3
	else
		# Make it harder for large misses to count
		y_rating = (200 ^ 2 + (y_diff - 200) ^ 3) / 3
	end

	return x_rating + y_rating
end

# Manually selected targets for each picture
# index = picture number; image1 does not exist
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

# Process each frame's data file
frames = []
for i in 2:20
	println("Processing image ", i)

	# Generate target delta
	local target_delta = target_to_delta(targets[i])
	println(target_delta)

	# Load from file
	local data = DataFrame(CSV.File(string("../data/image", i, ".csv")))

	# Calculate error rating
	data[!, "error_rating"] = map((x, y) -> error_rating(target_delta, (x, y)), data.delta_x, data.delta_y)

	# Add to array
	push!(frames, data)
end

# Combine all data into one dataframe
combined = frames[1]
for i in 2:19
	global combined = vcat(combined, frames[i])
end

# Group output by input parameters
grouped = groupby(combined, [:white, :black, :kernel, :iter, :max_area, :min_size])
# Filter any model that failed to detect anything at least once
filtered = filter(:white => w -> length(w) == 19, grouped)
# Combine groups to generate an average error rating
recombined = combine(filtered, :error_rating => mean)
# Sort by ascending error rating average
sort!(recombined, :error_rating_mean)

# Select first 100/1000 elements
smallest1000 = first(recombined, 1000)
smallest100 = first(recombined, 100)

# Columns to friendly name
column_map = Dict(
	:white => "White sensitivity",
	:black => "Black sensitivity",
	:kernel => "Kernel size",
	:iter => "Iterations",
	:max_area => "Maximum bounding box area",
	:min_size => "Minimum bounding box side length"
)

# Make plots
include("makeplots.jl")
for col in [:white, :black, :kernel, :iter, :max_area, :min_size]
	local averaged_total = collapse_data(recombined, col)
	local averaged_1000 = collapse_data(smallest1000, col)
	local averaged_100 = collapse_data(smallest100, col)

	local plots = make_plots(averaged_total, averaged_1000, averaged_100, column_map[col])

	savefig(plots[1], string(col, "_total.svg"))
	savefig(plots[2], string(col, "_top1000.svg"))
	savefig(plots[3], string(col, "_top100.svg"))
end
