/*************************************************************************/
/*  csg_gizmos.cpp                                                       */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2019 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2019 Godot Engine contributors (cf. AUTHORS.md)    */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/

#include "csg_gizmos.h"
#include "core/os/input.h"
#include "core/os/keyboard.h"
#include "csg_menu.h"

///////////

CSGShapeSpatialGizmoPlugin::CSGShapeSpatialGizmoPlugin() {
	set_extra_gizmos(false);
	first_set = true;

	Color gizmo_color = EDITOR_DEF("editors/3d_gizmos/gizmo_colors/csg", Color(0.0, 0.4, 1, 0.15));
	create_material("shape_union_material", gizmo_color);
	create_material("shape_union_solid_material", gizmo_color);
	gizmo_color.invert();
	create_material("shape_subtraction_material", gizmo_color);
	create_material("shape_subtraction_solid_material", gizmo_color);
	gizmo_color.r = 0.95;
	gizmo_color.g = 0.95;
	gizmo_color.b = 0.95;
	create_material("shape_intersection_material", gizmo_color);
	create_material("shape_intersection_solid_material", gizmo_color);

	create_handle_material("handles");
}

String CSGShapeSpatialGizmoPlugin::get_handle_name(const EditorSpatialGizmo *p_gizmo, int p_idx) const {

	CSGShape *cs = Object::cast_to<CSGShape>(p_gizmo->get_spatial_node());

	if (Object::cast_to<CSGSphere>(cs)) {

		return "Radius";
	}

	if (Object::cast_to<CSGBox>(cs)) {

		static const char *hname[3] = { "Width", "Height", "Depth" };
		return hname[p_idx % 3];
	}

	if (Object::cast_to<CSGCylinder>(cs)) {

		return p_idx == 0 ? "Radius" : "Height";
	}

	if (Object::cast_to<CSGTorus>(cs)) {

		return p_idx == 0 ? "InnerRadius" : "OuterRadius";
	}

	return "";
}
Variant CSGShapeSpatialGizmoPlugin::get_handle_value(EditorSpatialGizmo *p_gizmo, int p_idx) const {

	CSGShape *cs = Object::cast_to<CSGShape>(p_gizmo->get_spatial_node());

	if (Object::cast_to<CSGSphere>(cs)) {

		CSGSphere *s = Object::cast_to<CSGSphere>(cs);
		return s->get_radius();
	}

	if (Object::cast_to<CSGBox>(cs)) {

		CSGBox *s = Object::cast_to<CSGBox>(cs);
		switch (p_idx % 3) {
			case 0: return s->get_width();
			case 1: return s->get_height();
			case 2: return s->get_depth();
		}
	}

	if (Object::cast_to<CSGCylinder>(cs)) {

		CSGCylinder *s = Object::cast_to<CSGCylinder>(cs);
		return p_idx == 0 ? s->get_radius() : s->get_height();
	}

	if (Object::cast_to<CSGTorus>(cs)) {

		CSGTorus *s = Object::cast_to<CSGTorus>(cs);
		return p_idx == 0 ? s->get_inner_radius() : s->get_outer_radius();
	}

	return Variant();
}
void CSGShapeSpatialGizmoPlugin::set_handle(EditorSpatialGizmo *p_gizmo, int p_idx, Camera *p_camera, const Point2 &p_point) {

	CSGShape *cs = Object::cast_to<CSGShape>(p_gizmo->get_spatial_node());

	static Transform gt;
	static Transform lt;
	if (first_set) {
		menu_panel->get_check_box()->set_disabled(true);
		gt = cs->get_global_transform();
		lt = cs->get_transform();
		old_transform = lt;
	}
	//gt.orthonormalize();
	Transform gi = gt.affine_inverse();
	Transform li = lt.affine_inverse();

	Vector3 ray_from = p_camera->project_ray_origin(p_point);
	Vector3 ray_dir = p_camera->project_ray_normal(p_point);

	Vector3 sg[2] = { gi.xform(ray_from), gi.xform(ray_from + ray_dir * 16384) };
	Vector3 sl[2] = { li.xform(ray_from), li.xform(ray_from + ray_dir * 16384) };

	if (Object::cast_to<CSGSphere>(cs)) {

		CSGSphere *s = Object::cast_to<CSGSphere>(cs);

		Vector3 axis;
		axis[p_idx % 3] = 1.0;
		Vector3 ra, rb;
		if (extra_gizmos) {
			Geometry::get_closest_points_between_segments(Vector3(), axis * 4096, sl[0], sl[1], ra, rb);
		} else {
			Geometry::get_closest_points_between_segments(Vector3(), axis * 4096, sg[0], sg[1], ra, rb);
		}
		float d = rb[p_idx % 3];
		d = Math::abs(d);
		if (SpatialEditor::get_singleton()->is_snap_enabled()) {
			d = Math::stepify(d, SpatialEditor::get_singleton()->get_translate_snap());
		}

		int sign = (p_idx > 2) ? -1 : 1;
		if (first_set && extra_gizmos)
			d = d * 2;
		if (d < 0.001)
			d = 0.001;
		if (extra_gizmos) {
			d = d * 0.5;
			switch (p_idx % 3) {
				case 0:
					if (first_set) {
						lt = lt.translated(Vector3(s->get_radius() * -sign, 0, 0));
					}
					s->set_radius(d);
					s->set_transform(lt.translated(Vector3(d * sign, 0, 0)));
					break;
				case 1:
					if (first_set) {
						lt = lt.translated(Vector3(0, s->get_radius() * -sign, 0));
					}
					s->set_radius(d);
					s->set_transform(lt.translated(Vector3(0, d * sign, 0)));
					break;
				case 2:
					if (first_set) {
						lt = lt.translated(Vector3(0, 0, s->get_radius() * -sign));
					}
					s->set_radius(d);
					s->set_transform(lt.translated(Vector3(0, 0, d * sign)));
					break;
			}
		} else {
			s->set_radius(d);
		}
	}

	if (Object::cast_to<CSGBox>(cs)) {

		CSGBox *s = Object::cast_to<CSGBox>(cs);

		Vector3 axis;
		axis[p_idx % 3] = 1.0;
		Vector3 ra, rb;
		if (extra_gizmos) {
			Geometry::get_closest_points_between_segments(Vector3(), axis * 4096, sl[0], sl[1], ra, rb);
		} else {
			Geometry::get_closest_points_between_segments(Vector3(), axis * 4096, sg[0], sg[1], ra, rb);
		}
		float d = rb[p_idx % 3];
		d = Math::abs(d);
		if (SpatialEditor::get_singleton()->is_snap_enabled()) {
			d = Math::stepify(d, SpatialEditor::get_singleton()->get_translate_snap());
		}
		int sign = (p_idx > 2) ? -1 : 1;
		if (first_set && extra_gizmos)
			d = d * 2;
		if (d < 0.001)
			d = 0.001;

		switch (p_idx % 3) {
			case 0:
				if (first_set && extra_gizmos) {
					lt = lt.translated(Vector3(s->get_width() * 0.5 * -sign, 0, 0));
				}
				if (extra_gizmos) {
					s->set_width(d);
					s->set_transform(lt.translated(Vector3(d * 0.5 * sign, 0, 0)));
				} else {
					s->set_width(d * 2);
				}
				break;
			case 1:
				if (first_set && extra_gizmos) {
					lt = lt.translated(Vector3(0, s->get_height() * 0.5 * -sign, 0));
				}
				if (extra_gizmos) {
					s->set_height(d);
					s->set_transform(lt.translated(Vector3(0, d * 0.5 * sign, 0)));
				} else {
					s->set_height(d * 2);
				}
				break;
			case 2:
				if (first_set && extra_gizmos) {
					lt = lt.translated(Vector3(0, 0, s->get_depth() * 0.5 * -sign));
				}
				if (extra_gizmos) {
					s->set_depth(d);
					s->set_transform(lt.translated(Vector3(0, 0, d * 0.5 * sign)));
				} else {
					s->set_depth(d * 2);
				}
				break;
		}
	}

	if (Object::cast_to<CSGCylinder>(cs)) {

		CSGCylinder *s = Object::cast_to<CSGCylinder>(cs);

		Vector3 axis;
		axis[p_idx == 0 ? 0 : 1] = 1.0;
		Vector3 ra, rb;
		Geometry::get_closest_points_between_segments(Vector3(), axis * 4096, sg[0], sg[1], ra, rb);
		float d = axis.dot(ra);
		if (SpatialEditor::get_singleton()->is_snap_enabled()) {
			d = Math::stepify(d, SpatialEditor::get_singleton()->get_translate_snap());
		}

		if (d < 0.001)
			d = 0.001;

		if (p_idx == 0)
			s->set_radius(d);
		else if (p_idx == 1)
			s->set_height(d * 2.0);
	}

	if (Object::cast_to<CSGTorus>(cs)) {

		CSGTorus *s = Object::cast_to<CSGTorus>(cs);

		Vector3 axis;
		axis[0] = 1.0;
		Vector3 ra, rb;
		Geometry::get_closest_points_between_segments(Vector3(), axis * 4096, sg[0], sg[1], ra, rb);
		float d = axis.dot(ra);
		if (SpatialEditor::get_singleton()->is_snap_enabled()) {
			d = Math::stepify(d, SpatialEditor::get_singleton()->get_translate_snap());
		}

		if (d < 0.001)
			d = 0.001;

		if (p_idx == 0)
			s->set_inner_radius(d);
		else if (p_idx == 1)
			s->set_outer_radius(d);
	}

	first_set = false;
}
void CSGShapeSpatialGizmoPlugin::commit_handle(EditorSpatialGizmo *p_gizmo, int p_idx, const Variant &p_restore, bool p_cancel) {

	first_set = true;
	menu_panel->get_check_box()->set_disabled(false);

	CSGShape *cs = Object::cast_to<CSGShape>(p_gizmo->get_spatial_node());

	if (Object::cast_to<CSGSphere>(cs)) {
		CSGSphere *s = Object::cast_to<CSGSphere>(cs);
		if (p_cancel) {
			s->set_radius(p_restore);
			return;
		}

		UndoRedo *ur = SpatialEditor::get_singleton()->get_undo_redo();
		ur->create_action(TTR("Change Sphere Shape Radius"));
		ur->add_do_method(s, "set_radius", s->get_radius());
		if (extra_gizmos) {
			ur->add_do_method(s, "set_transform", s->get_transform());
		}
		ur->add_undo_method(s, "set_radius", p_restore);
		if (extra_gizmos) {
			ur->add_undo_method(s, "set_transform", old_transform);
		}
		ur->commit_action();
	}

	if (Object::cast_to<CSGBox>(cs)) {
		CSGBox *s = Object::cast_to<CSGBox>(cs);
		if (p_cancel) {
			switch (p_idx % 3) {
				case 0: s->set_width(p_restore); break;
				case 1: s->set_height(p_restore); break;
				case 2: s->set_depth(p_restore); break;
			}
			return;
		}

		UndoRedo *ur = SpatialEditor::get_singleton()->get_undo_redo();
		ur->create_action(TTR("Change Box Shape Extents"));
		static const char *method[3] = { "set_width", "set_height", "set_depth" };
		float current = 0;
		switch (p_idx % 3) {
			case 0: current = s->get_width(); break;
			case 1: current = s->get_height(); break;
			case 2: current = s->get_depth(); break;
		}

		ur->add_do_method(s, method[p_idx % 3], current);
		if (extra_gizmos) {
			ur->add_do_method(s, "set_transform", s->get_transform());
		}
		ur->add_undo_method(s, method[p_idx % 3], p_restore);
		if (extra_gizmos) {
			ur->add_undo_method(s, "set_transform", old_transform);
		}

		ur->commit_action();
	}

	if (Object::cast_to<CSGCylinder>(cs)) {
		CSGCylinder *s = Object::cast_to<CSGCylinder>(cs);
		if (p_cancel) {
			if (p_idx == 0)
				s->set_radius(p_restore);
			else
				s->set_height(p_restore);
			return;
		}

		UndoRedo *ur = SpatialEditor::get_singleton()->get_undo_redo();
		if (p_idx == 0) {
			ur->create_action(TTR("Change Cylinder Radius"));
			ur->add_do_method(s, "set_radius", s->get_radius());
			ur->add_undo_method(s, "set_radius", p_restore);
		} else {
			ur->create_action(TTR("Change Cylinder Height"));
			ur->add_do_method(s, "set_height", s->get_height());
			ur->add_undo_method(s, "set_height", p_restore);
		}

		ur->commit_action();
	}

	if (Object::cast_to<CSGTorus>(cs)) {
		CSGTorus *s = Object::cast_to<CSGTorus>(cs);
		if (p_cancel) {
			if (p_idx == 0)
				s->set_inner_radius(p_restore);
			else
				s->set_outer_radius(p_restore);
			return;
		}

		UndoRedo *ur = SpatialEditor::get_singleton()->get_undo_redo();
		if (p_idx == 0) {
			ur->create_action(TTR("Change Torus Inner Radius"));
			ur->add_do_method(s, "set_inner_radius", s->get_inner_radius());
			ur->add_undo_method(s, "set_inner_radius", p_restore);
		} else {
			ur->create_action(TTR("Change Torus Outer Radius"));
			ur->add_do_method(s, "set_outer_radius", s->get_outer_radius());
			ur->add_undo_method(s, "set_outer_radius", p_restore);
		}

		ur->commit_action();
	}
}
bool CSGShapeSpatialGizmoPlugin::has_gizmo(Spatial *p_spatial) {
	return Object::cast_to<CSGSphere>(p_spatial) || Object::cast_to<CSGBox>(p_spatial) || Object::cast_to<CSGCylinder>(p_spatial) || Object::cast_to<CSGTorus>(p_spatial) || Object::cast_to<CSGMesh>(p_spatial) || Object::cast_to<CSGPolygon>(p_spatial);
}

String CSGShapeSpatialGizmoPlugin::get_name() const {
	return "CSGShapes";
}

int CSGShapeSpatialGizmoPlugin::get_priority() const {
	return -1;
}

void CSGShapeSpatialGizmoPlugin::set_extra_gizmos(bool p_extra_gizmos) {
	extra_gizmos = p_extra_gizmos;
}

void CSGShapeSpatialGizmoPlugin::set_menu_panel(CSGShapeMenuPanelPlugin *p_menu_panel) {
	menu_panel = p_menu_panel;
}

bool CSGShapeSpatialGizmoPlugin::is_selectable_when_hidden() const {
	return true;
}

void CSGShapeSpatialGizmoPlugin::redraw(EditorSpatialGizmo *p_gizmo) {

	CSGShape *cs = Object::cast_to<CSGShape>(p_gizmo->get_spatial_node());

	p_gizmo->clear();

	Ref<Material> material;
	switch (cs->get_operation()) {
		case CSGShape::OPERATION_UNION:
			material = get_material("shape_union_material", p_gizmo);
			break;
		case CSGShape::OPERATION_INTERSECTION:
			material = get_material("shape_intersection_material", p_gizmo);
			break;
		case CSGShape::OPERATION_SUBTRACTION:
			material = get_material("shape_subtraction_material", p_gizmo);
			break;
	}

	Ref<Material> handles_material = get_material("handles");

	PoolVector<Vector3> faces = cs->get_brush_faces();

	Vector<Vector3> lines;
	lines.resize(faces.size() * 2);
	{
		PoolVector<Vector3>::Read r = faces.read();

		for (int i = 0; i < lines.size(); i += 6) {
			int f = i / 6;
			for (int j = 0; j < 3; j++) {
				int j_n = (j + 1) % 3;
				lines.write[i + j * 2 + 0] = r[f * 3 + j];
				lines.write[i + j * 2 + 1] = r[f * 3 + j_n];
			}
		}
	}

	p_gizmo->add_lines(lines, material);
	p_gizmo->add_collision_segments(lines);

	if (p_gizmo->is_selected()) {
		// Draw a translucent representation of the CSG node
		Ref<ArrayMesh> mesh = memnew(ArrayMesh);
		Array array;
		array.resize(Mesh::ARRAY_MAX);
		array[Mesh::ARRAY_VERTEX] = faces;
		mesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, array);

		Ref<Material> solid_material;
		switch (cs->get_operation()) {
			case CSGShape::OPERATION_UNION:
				solid_material = get_material("shape_union_solid_material", p_gizmo);
				break;
			case CSGShape::OPERATION_INTERSECTION:
				solid_material = get_material("shape_intersection_solid_material", p_gizmo);
				break;
			case CSGShape::OPERATION_SUBTRACTION:
				solid_material = get_material("shape_subtraction_solid_material", p_gizmo);
				break;
		}

		p_gizmo->add_mesh(mesh, false, Ref<SkinReference>(), solid_material);
	}

	if (Object::cast_to<CSGSphere>(cs)) {
		CSGSphere *s = Object::cast_to<CSGSphere>(cs);

		float r = s->get_radius();
		Vector<Vector3> handles;
		handles.push_back(Vector3(r, 0, 0));
		if (extra_gizmos) {
			handles.push_back(Vector3(0, r, 0));
			handles.push_back(Vector3(0, 0, r));
			handles.push_back(Vector3(-r, 0, 0));
			handles.push_back(Vector3(0, -r, 0));
			handles.push_back(Vector3(0, 0, -r));
		}
		p_gizmo->add_handles(handles, handles_material);
	}

	if (Object::cast_to<CSGBox>(cs)) {
		CSGBox *s = Object::cast_to<CSGBox>(cs);

		Vector<Vector3> handles;
		handles.push_back(Vector3(s->get_width() * 0.5, 0, 0));
		handles.push_back(Vector3(0, s->get_height() * 0.5, 0));
		handles.push_back(Vector3(0, 0, s->get_depth() * 0.5));
		if (extra_gizmos) {
			handles.push_back(Vector3(s->get_width() * -0.5, 0, 0));
			handles.push_back(Vector3(0, s->get_height() * -0.5, 0));
			handles.push_back(Vector3(0, 0, s->get_depth() * -0.5));
		}
		p_gizmo->add_handles(handles, handles_material);
	}

	if (Object::cast_to<CSGCylinder>(cs)) {
		CSGCylinder *s = Object::cast_to<CSGCylinder>(cs);

		Vector<Vector3> handles;
		handles.push_back(Vector3(s->get_radius(), 0, 0));
		handles.push_back(Vector3(0, s->get_height() * 0.5, 0));
		p_gizmo->add_handles(handles, handles_material);
	}

	if (Object::cast_to<CSGTorus>(cs)) {
		CSGTorus *s = Object::cast_to<CSGTorus>(cs);

		Vector<Vector3> handles;
		handles.push_back(Vector3(s->get_inner_radius(), 0, 0));
		handles.push_back(Vector3(s->get_outer_radius(), 0, 0));
		p_gizmo->add_handles(handles, handles_material);
	}
}

void EditorPluginCSG::make_visible(bool p_visible) {
	if (p_visible) {
		menu_panel->show();
	} else {
		menu_panel->hide();
	}
}

bool EditorPluginCSG::handles(Object *p_object) const {
	return Object::cast_to<CSGPrimitive>(p_object);
}

EditorPluginCSG::EditorPluginCSG(EditorNode *p_editor) {
	Ref<CSGShapeSpatialGizmoPlugin> gizmo_plugin = Ref<CSGShapeSpatialGizmoPlugin>(memnew(CSGShapeSpatialGizmoPlugin));
	CSGShapeMenuPanelPlugin *menu_plugin = memnew(CSGShapeMenuPanelPlugin(p_editor));
	gizmo_plugin->set_menu_panel(menu_plugin);
	menu_panel = menu_plugin;
	menu_panel->set_gizmos(*gizmo_plugin);
	SpatialEditor::get_singleton()->add_gizmo_plugin(gizmo_plugin);
	SpatialEditor::get_singleton()->add_control_to_menu_panel(menu_panel);
	menu_panel->hide();
}
