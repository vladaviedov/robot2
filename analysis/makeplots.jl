using Plots
using DataFrames
using Statistics
using CurveFit

function make_one_plot(
	data::Tuple{Vector{Int64}, Vector{Float64}},
	xlabel::String,
	title::String
)::Plots.Plot
	local p = plot(
		data[1],
		data[2],
		xlabel=xlabel,
		ylabel="Average Error Rating",
		label="data",
		title=title,
		seriestype=:scatter
	)

	local regression = linear_fit(data[1], data[2])
	local regression_func = x -> regression[1] + regression[2] * x
	
	local label = string(
		round(regression[1]; sigdigits=4),
		" + ",
		round(regression[2]; sigdigits=4),
		"x"
	)

	plot!(p,
		regression_func,
		label=label
	)
end

function make_plots(
	total::Tuple{Vector{Int64}, Vector{Float64}},
	top1000::Tuple{Vector{Int64}, Vector{Float64}},
	top100::Tuple{Vector{Int64}, Vector{Float64}},
	xlabel::String,
)::Tuple{Plots.Plot, Plots.Plot, Plots.Plot}
	ptotal = make_one_plot(total, xlabel, "Total")
	p1000 = make_one_plot(top1000, xlabel, "Top 1000")
	p100 = make_one_plot(top100, xlabel, "Top 100")

	return (ptotal, p1000, p100)
end

function collapse_data(
	data::DataFrames.DataFrame,
	column1::Symbol,
)::Tuple{Vector{Int64}, Vector{Float64}}
	local grouped = groupby(data, [column1])
	local combined = combine(grouped, :error_rating_mean => mean => :error_rating)
	return (combined[!, column1], combined[!, :error_rating])
end
