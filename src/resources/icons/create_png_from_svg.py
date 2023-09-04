import argparse
import cairosvg
from PIL import Image

# Define a function to render SVG to PNG with specified dimensions
def render_svg_to_png(input_svg, output_png, width, height):
    cairosvg.svg2png(url=input_svg, write_to=output_png, output_width=width, output_height=height)
    image = Image.open(output_png)
    image = image.resize((width, height))
    image.save(output_png)

def main():
    # Create an argument parser
    parser = argparse.ArgumentParser(description="Render SVG to PNG with specified dimensions")

    # Add arguments for input SVG, base name of output PNG, width, and height
    parser.add_argument("input_svg", help="Input SVG file")
    parser.add_argument("output_base", help="Base name of the output PNG file (without dimensions)")
    parser.add_argument("--size", type=int, required=False, help="Size of the PNG")

    # Parse the command-line arguments
    args = parser.parse_args()

    # Create the output PNG file name with dimensions
    output_png = f"{args.output_base}.png" if args.size is None else f"{args.output_base}_{args.size}.png"

    if args.size is None: args.size = 64

    # Render the SVG to PNG with specified dimensions
    render_svg_to_png(args.input_svg, output_png, args.size, args.size)

    print(f"SVG rendered to PNG with dimensions {args.size}x{args.size} and saved to {output_png}")

if __name__ == "__main__":
    main()
